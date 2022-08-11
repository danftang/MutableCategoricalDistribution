//
// Created by daniel on 08/08/22.
//
// This class represents a categorical distribution over an arbitrary list of objects
// [C_0...C_N] of some type T. Each object is associated with a weight {w_0...w_N}
// and the probability of each object is proportional to its weight so that
//
// P(C_i) = w_i / \sum_j w_j
//
// The list [C_0...C_N] need not consist of unique objects (i.e. it is possible that C_i == C_j
// for some i \ne j).
//
// The underlying storage is a MutableCategoricalArray, along with another array
// that maps the integer range [0...N] to {C_0...C_N}.
#ifndef CPP_MUTABLECATEGORICAL_H
#define CPP_MUTABLECATEGORICAL_H

#include <list>
#include "MutableCategoricalArray.h"

template<class T>
class MutableCategorical {
protected:

    class Category {
    public:
        T value;

        operator const T &() const { return value; }
        operator T &() & { return value; }
        operator T &&() && { return std::move(value); }

        Category(const T &v, int index) : value(v), index(index) {}
        Category(T &&v, int index) : value(std::move(v)), index(index) {}
        int index;
    };


    template<class V>
    class iterator_base {
    public:

        iterator_base(V ptr): ptr(ptr) {}

        auto &operator *()  { return ptr->value; }
        auto *operator ->() { return &ptr->value; }
        iterator_base<V> &operator ++() { ++ptr; return *this; }
        iterator_base<V> operator ++(int) { return ptr++; }
        bool operator ==(const iterator_base<V> &other) const { return ptr == other.ptr; }
        bool operator !=(const iterator_base<V> &other) const { return ptr != other.ptr; }
        operator iterator_base<typename std::list<Category>::const_iterator>() { return iterator_base<typename std::list<Category>::const_iterator>(ptr); }

    protected:
        V ptr;
        auto &index() { return ptr->index; }
        friend class MutableCategorical<T>;
    };


public:
    typedef T value_type;
    typedef iterator_base<typename std::list<Category>::iterator>        iterator;
    typedef iterator_base<typename std::list<Category>::const_iterator>  const_iterator;

    MutableCategoricalArray mca;
    std::vector<iterator>   indexToCategory;
    std::list<Category>     categories;


    MutableCategorical() {}

    MutableCategorical(int size, std::function<std::pair<T,double>(int)> init) {
        mca.reserve(size);
        indexToCategory.reserve(size);
        for(int i=size-1; i>=0; --i) {
            std::pair<T,double> v = init(i);
            add(std::move(v.first), v.second);
        }
    }


    iterator add(const T &categoryLabel, double weight);
    iterator add(T &&categoryLabel, double weight);
    iterator erase(iterator category);
    void set(iterator category, double weight);
    double weight(const_iterator category) const { return mca[category.index()]; }
    double probability(const_iterator category) const { return weight(category)/sum(); }
    double sum() const { return mca.sum(); }
    iterator begin() { return categories.begin(); }
    iterator end()   { return categories.end(); }
    const_iterator begin() const { return categories.begin(); }
    const_iterator end()   const { return categories.end(); }
    size_t size() const { return indexToCategory.size(); }
    void reserve(size_t size) { indexToCategory.reserve(size); mca.reserve(size); }
    template<class RNG> iterator operator()(RNG &randomGenerator) {
        if(size() == 0) return categories.end();
        return indexToCategory[mca(randomGenerator)];
    }
    template<class RNG> const_iterator operator()(RNG &randomGenerator) const {
        if(size() == 0) return categories.end();
        return indexToCategory[mca(randomGenerator)];
    }


    friend std::ostream &operator <<(std::ostream &out, const MutableCategorical<T> &distribution) {
        for(int i=0; i<distribution.size(); ++i) {
            out << distribution.categoryLabels[i] << " -> " << distribution.mca[i] << std::endl;
        }
        return out;
    }
};

// invalidates the erased iterator.
// returns an iterator to the element after the erased element.
template<class T>
typename MutableCategorical<T>::iterator MutableCategorical<T>::erase(iterator category) {
    int categoryIndexToErase = category.index();
    int lastCategoryIndex = size() - 1;
    if(categoryIndexToErase != lastCategoryIndex) {
        mca.set(categoryIndexToErase, mca[lastCategoryIndex]);
        iterator categoryToReindex = indexToCategory[lastCategoryIndex];
        indexToCategory[categoryIndexToErase] = categoryToReindex;
        categoryToReindex.index() = categoryIndexToErase;
    }
    mca.pop_back();
    indexToCategory.pop_back();
    return categories.erase(category.ptr);
}

template<class T>
typename MutableCategorical<T>::iterator MutableCategorical<T>::add(const T &categoryLabel, double weight) {
    mca.push_back(weight);
    categories.push_front(Category(categoryLabel, mca.size()-1));
    indexToCategory.push_back(categories.begin());
    return categories.begin();
}

template<class T>
typename MutableCategorical<T>::iterator MutableCategorical<T>::add(T &&categoryLabel, double weight) {
    mca.push_back(weight);
    categories.push_front(Category(std::move(categoryLabel), mca.size()-1));
    indexToCategory.push_back(categories.begin());
    return categories.begin();
}

template<class T>
void MutableCategorical<T>::set(iterator category, double weight) {
    mca.set(category.index(), weight);
}


#endif //CPP_MUTABLECATEGORICAL_H
