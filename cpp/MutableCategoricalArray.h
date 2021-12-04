
#ifndef CPP_MUTABLECATEGORICALARRAY_H
#define CPP_MUTABLECATEGORICALARRAY_H

#include <functional>
#include <array>
#include <random>

// Represents a probability distribution over an integer range 0..N
// Can be treated as an array of doubles, where each double is an
// (un-normalised) probability for that index.
//
// Internally this is stored as a binary sum tree. However, we
// only store sums of nodes that are right hand children. This allows
// us to map the tree onto an array of doubles. A tree node is mapped to an array entry
// whose index can be calculated, starting from the most significant bit, by following the sequence
// of left/right (0/1) branches on the path from the root to that node.
// Since the path must end with a right branch (1) we pad the sequence with
// zeroes beyond the final right branch to give a unique index.
//
// This encoding allows arrays of any size (not just integer multiples of 2)
// and allows modification of probabilities and sampling in O(log(N)) time.
// If all probabilities need modifying, this can be done in O(N) time using
// the setAll method.
class MutableCategoricalArray {


    class EntryRef {
        int i;
        MutableCategoricalArray &p;
    public:

        EntryRef(int index, MutableCategoricalArray &dist): i(index), p(dist) { }
        operator double() { return p.get(i); }
        double operator =(double probability) { p.set(i,probability); return probability; }
    };


    std::vector<double> tree;
    int indexHighestBit;
    std::uniform_real_distribution<double> uniformDist;

public:


    MutableCategoricalArray(int size): tree(size), uniformDist(0.0,1.0) {
        indexHighestBit = highestOneBit(size-1);
    }


    MutableCategoricalArray(int size, std::function<double(int)> init): MutableCategoricalArray(size) {
        for(int i=size-1; i>=0; --i) {
            tree[i] = descendantSum(i) + init(i);
        }
    }

    MutableCategoricalArray(std::initializer_list<double> values): MutableCategoricalArray(values.size()) {
        setAll(values);
    }


    int size() const { return tree.size(); }


    // sets the un-normalised probability associated with the supplied index
    EntryRef operator [](int index) { return EntryRef(index, *this); }


    // returns the un-normalised probability associated with the supplied index.
    double operator [](int index) const { return get(index); }

    double get(int index) const { return tree[index] - descendantSum(index); }

    void set(int index, double probability) {
        double sum = probability;
        int indexOffset = 1;
        while((indexOffset & index) == 0 && indexOffset < size()) {
            int descendantIndex = index + indexOffset;
            if(descendantIndex < size()) sum += tree[descendantIndex];
            indexOffset = indexOffset << 1;
        }
        double delta = sum - tree[index];
        int ancestorIndex = index;
        tree[index] = sum;
        while(indexOffset < size()) {
            ancestorIndex = ancestorIndex ^ indexOffset;
            tree[ancestorIndex] += delta;
            do {
                indexOffset = indexOffset << 1;
            } while((ancestorIndex & indexOffset) == 0 && indexOffset < size());
        }
    }

    // draws a sample from the distribution
    template<typename RNG>
    int operator()(RNG &generator) {
        int index = 0;
        double target = uniformDist(generator) * tree[0];
        int rightChildOffset = indexHighestBit;
        while(rightChildOffset != 0) {
            int childIndex = index+rightChildOffset;
            if(childIndex < size()) {
                if (tree[childIndex] > target) index += rightChildOffset; else target -= tree[childIndex];
            }
            rightChildOffset = rightChildOffset >> 1;
        }
        return index;
    }


    // Sets the un-normalised probabilities of the first N integers
    // Runs in O(N) time since descendantSum runs in amortized constant
    // time (simce average number of steps is 2 for any size of tree).
    template<typename RANDOMACCESSCONTAINER>
    void setAll(const RANDOMACCESSCONTAINER &values) {
        for(int i = values.size()-1; i>=0; --i) {
            tree[i] = descendantSum(i) + values[i];
        }
    }

    void setAll(std::initializer_list<double> values) {
        auto it = std::rbegin(values);
        for(int i = values.size()-1; i>=0; --i) {
            tree[i] = descendantSum(i) + *it++;
        }
    }


    // the sum of all un-normalised probabilities (doesn't need to be 1.0)
    double sum() {
        return tree[0];
    }


    // Returns the normalised probability of the index'th element
    double P(int index) { return (*this)[index]/sum(); }


//    fun asList(): List<Double> {
//        return object: AbstractList<Double>() {
//            override val size: Int
//                get() = this@MutableCategoricalArray.size
//            override fun get(index: Int) = this@MutableCategoricalArray[index]
//        }
//    }

protected:

    double descendantSum(int index) const {
        int indexOffset = 1;
        double sum = 0.0;
        while((indexOffset & index) == 0 && indexOffset < size()) {
            int descendantIndex = index + indexOffset;
            if(descendantIndex < size()) sum += tree[descendantIndex];
            indexOffset = indexOffset << 1;
        }
        return sum;
    }


    static int highestOneBit(int i) {
        i = i | (i >> 1);
        i = i | (i >> 2);
        i = i | (i >> 4);
        i = i | (i >> 8);
        i = i | (i >> 16);
        return i - (i >> 1);
    }
};

#endif //CPP_MUTABLECATEGORICALARRAY_H
