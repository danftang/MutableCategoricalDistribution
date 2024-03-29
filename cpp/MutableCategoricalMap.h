//
// Created by daniel on 02/08/22.
//
// This class represents a categorical distribution over an arbitrary set of objects
// {C_1...C_N} of some type T. Each object is associated with a weight {w_1...w_N}
// and the probability of each object is proportional to its weight so that
//
// P(C_i) = w_i / \sum_j w_j
//
// The class can be thought of as a container of Category objects, where each category
// contains an object of type T and its associated weight. Categories can be added
// and removed from the container using add() and erase(), their weights can be modified
// using category.setWeight(), or a category can be drawn at random using the
// call operator (), all in O(log(N)) time.
// The sum of all weights can be accessed in O(1) time using sum()
#ifndef CPP_MUTABLECATEGORICALMAP_H
#define CPP_MUTABLECATEGORICALMAP_H

#include <assert.h>
#include <deque>
#include <random>

template<class T>
class MutableCategoricalMap {
protected:
    static std::mt19937 random;

    class SumTreeNode {
    public:
        double          sum;
        SumTreeNode *   parent;
        SumTreeNode *   leftChild;
        SumTreeNode *   rightChild;

        SumTreeNode(SumTreeNode *parent, SumTreeNode *leftChild, SumTreeNode *rightChild, double sum):
            parent(parent), leftChild(leftChild), rightChild(rightChild), sum(sum)
        {}

        bool isLeaf() const { return leftChild == nullptr; }
        void updateSum() { sum = leftChild->sum + rightChild->sum; }

        SumTreeNode *siblingOf(const SumTreeNode &child) const {
            assert(&child == leftChild || &child == rightChild);
            return &child == leftChild?rightChild:leftChild;
        }

        void updateChild(SumTreeNode *oldChild, SumTreeNode *newChild) {
            assert(oldChild == leftChild || oldChild == rightChild);
            if(oldChild == leftChild) {
                leftChild = newChild;
            } else {
                rightChild = newChild;
            }
        }

        void updateAncestorSums();

    };


    template<class V>
    class iterator_base {
    public:
        typedef V              value_type;
        typedef value_type &   reference;
        typedef value_type *   pointer;

        pointer ptr;

        iterator_base(pointer ptr): ptr(ptr) {}
        iterator_base(): ptr(nullptr) {}

        reference operator *() { return *ptr; }
        pointer operator ->() { return ptr; }
        iterator_base<V> &operator ++();
        iterator_base<V> operator ++(int) { iterator preIncrementVal(ptr); ++(*this); return preIncrementVal; }
        bool operator ==(const iterator_base<V> &other) const { return ptr == other.ptr; }
        bool operator !=(const iterator_base<V> &other) const { return ptr != other.ptr; }
        operator iterator_base<const V>() { return iterator_base<const V>(ptr); }
    };



public:
    class Category: protected SumTreeNode {
    public:
        template<class V>
        Category(V &&categoryValue, SumTreeNode *parent, double probability) :
                value(std::forward<V>(categoryValue)), SumTreeNode(parent, nullptr, nullptr, probability) { }

        T value;
        double getWeight() const { return this->sum; }
        void setWeight(double w) { this->sum = w; this->updateAncestorSums(); }
        operator T() { return value; }
        operator const T() const { return value; }
        friend class MutableCategoricalMap<T>;

    protected:
        SumTreeNode *nodePtr() { return this; }
        const SumTreeNode *nodePtr() const { return this; }

    };

    typedef iterator_base<Category>         iterator;
    typedef iterator_base<const Category>   const_iterator;

    MutableCategoricalMap(): rootNode(nullptr), nCategories(0) {
    }

    ~MutableCategoricalMap() { clear(); }

    iterator add(const T &categoryValue, double probability);
    iterator erase(const_iterator category);
    template<typename RNG = decltype(random)> iterator operator ()(RNG &randomGenerator=random) { return choose<iterator>(*this, randomGenerator); }
    template<typename RNG = decltype(random)> const_iterator operator()(RNG &randomGenerator=random) const { return choose<const_iterator>(*this, randomGenerator); }
    double sum() const { return (rootNode == nullptr)?0.0:rootNode->sum; }
    double probability(const_iterator category) const { return category->getWeight()/sum(); }
    static double weight(const_iterator category) { return category->getWeight(); }
    static void set(iterator category, double weight) { category->setWeight(weight); }
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    void clear();
    int  size() { return nCategories; }

//    friend std::ostream &operator <<(std::ostream &out, const MutableCategorical<T> &mutableCategorical);

protected:
    SumTreeNode *   rootNode;
    int             nCategories;

    void insert(SumTreeNode &newNode, SumTreeNode &insertionPoint);
    template<class R, class V, class G> static R choose(V &distribution, G &randomGenerator);
};

template<class T>
template<class V>
typename MutableCategoricalMap<T>::template iterator_base<V> &MutableCategoricalMap<T>::iterator_base<V>::operator++() {
    if(ptr == nullptr) return *this;
    auto currentNode = ptr->nodePtr();
    while(currentNode->parent != nullptr && currentNode->parent->rightChild == currentNode) {
        currentNode = currentNode->parent;
    }
    if(currentNode->parent == nullptr) {
        ptr = nullptr;
    } else {
        currentNode = currentNode->parent->rightChild;
        while (!currentNode->isLeaf()) {
            currentNode = currentNode->leftChild;
        }
        ptr = static_cast<pointer>(currentNode);
    }
    return *this;
}


// navigate down the tree, taking always the lower sum child until sum is less than
// the probability to add, or we reach a leaf.
template<class T>
typename MutableCategoricalMap<T>::iterator MutableCategoricalMap<T>::add(const T &categoryValue, double probability) {
    Category *newLeaf = new Category(categoryValue, nullptr, probability);
    if(rootNode == nullptr) {
        rootNode = newLeaf;
    } else {
        SumTreeNode *currentNode = rootNode;
        while(currentNode->sum > probability && !currentNode->isLeaf()) {
            if(currentNode->leftChild->sum < currentNode->rightChild->sum) {
                currentNode = currentNode->leftChild;
            } else {
                currentNode = currentNode->rightChild;
            }
        }
        insert(*newLeaf, *currentNode);
    }
    ++nCategories;
    return iterator(newLeaf);
}

// inserts newNode at insertionPoint by creating a new sum node whose children are the new
// node (right child) and the insertion point (left child) and whose parent is the original
// parent of insertionPoint
template<class T>
void MutableCategoricalMap<T>::insert(SumTreeNode &newNode, SumTreeNode &insertionPoint) {
    SumTreeNode *newParent = new SumTreeNode(insertionPoint.parent, &insertionPoint, &newNode, insertionPoint.sum + newNode.sum);
    insertionPoint.parent = newParent;
    newNode.parent = newParent;
    if(newParent->parent == nullptr) {
        rootNode = newParent;
    } else {
        newParent->parent->updateChild(&insertionPoint, newParent);
        newParent->updateAncestorSums();
    }
}


template<class T>
void MutableCategoricalMap<T>::SumTreeNode::updateAncestorSums() {
    SumTreeNode *currentNode = parent;
    while(currentNode != nullptr) {
        currentNode->updateSum();
        currentNode = currentNode->parent;
    }
}



template<class T>
template<class R, class V, class G>
R MutableCategoricalMap<T>::choose(V &distribution, G &randomGenerator) {
    if(distribution.rootNode == nullptr) return distribution.end();
    double target = std::uniform_real_distribution<double>()(randomGenerator) * distribution.rootNode->sum;
    SumTreeNode *currentNode = distribution.rootNode;
    while(!currentNode->isLeaf()) {
        if (currentNode->leftChild->sum > target) {
            currentNode = currentNode->leftChild;
        } else {
            target -= currentNode->leftChild->sum;
            currentNode = currentNode->rightChild;
        }
    }
    return R(static_cast<Category *>(currentNode));
}

// remove a given category by removing the parent of the category and
// replacing it with its sibling. Returns an iterator pointing to the
// element after the one removed.
template<class T>
typename MutableCategoricalMap<T>::iterator MutableCategoricalMap<T>::erase(MutableCategoricalMap<T>::const_iterator categoryIt) {
    iterator nextIterator(const_cast<Category *>(categoryIt.ptr));
    ++nextIterator;
    SumTreeNode *parentToRemove = categoryIt->parent;
    if(parentToRemove == nullptr) {
        rootNode = nullptr;
    } else {
        SumTreeNode *sibling = parentToRemove->siblingOf(*categoryIt);
        sibling->parent = parentToRemove->parent;
        if(parentToRemove->parent == nullptr) {
            rootNode = sibling;
        } else {
            parentToRemove->parent->updateChild(parentToRemove, sibling);
            sibling->updateAncestorSums();
        }
        delete(parentToRemove);
    }
    delete(categoryIt.ptr);
    --nCategories;
    return nextIterator;
}

template<class T>
typename MutableCategoricalMap<T>::iterator MutableCategoricalMap<T>::begin() {
    if(rootNode == nullptr) return iterator(nullptr);
    SumTreeNode *currentNode = rootNode;
    while(!currentNode->isLeaf()) currentNode = currentNode->leftChild;
    return iterator(static_cast<Category *>(currentNode));
}

template<class T>
typename MutableCategoricalMap<T>::iterator MutableCategoricalMap<T>::end() {
    return MutableCategoricalMap::iterator(nullptr);
}

template<class T>
typename MutableCategoricalMap<T>::const_iterator MutableCategoricalMap<T>::begin() const {
    if(rootNode == nullptr) return const_iterator(nullptr);
    SumTreeNode *currentNode = rootNode;
    while(!currentNode->isLeaf()) currentNode = currentNode->leftChild;
    return const_iterator(static_cast<const Category *>(currentNode));
}

template<class T>
typename MutableCategoricalMap<T>::const_iterator MutableCategoricalMap<T>::end() const {
    return MutableCategoricalMap::const_iterator(nullptr);
}


template<class T>
void MutableCategoricalMap<T>::clear() {
    if(rootNode == nullptr) return;
    std::deque<SumTreeNode *> nodesToDelete;
    nodesToDelete.push_back(rootNode);
    while(!nodesToDelete.empty()) {
        SumTreeNode *nextNodeToDelete = nodesToDelete.front();
        nodesToDelete.pop_front();
        if(!nextNodeToDelete->isLeaf()) {
            nodesToDelete.push_back(nextNodeToDelete->leftChild);
            nodesToDelete.push_back(nextNodeToDelete->rightChild);
        }
        delete(nextNodeToDelete);
    }
    rootNode = nullptr;
}

template<class T>
std::ostream &operator<<(std::ostream &out, const MutableCategoricalMap<T> &mutableCategorical) {
    for(const typename MutableCategoricalMap<T>::Category &category : mutableCategorical) {
        out << category.value << " -> " << category.getWeight() << std::endl;
    }
    return out;
}

template<class T> std::mt19937 MutableCategoricalMap<T>::random;

#endif //CPP_MUTABLECATEGORICALMAP_H
