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

#include "MutableCategoricalArray.h"

template<class T>
class MutableCategorical {
protected:
    MutableCategoricalArray mca;
    std::vector<T>          categoryLabels;

public:
    typedef T value_type;
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

    MutableCategorical() {}

    MutableCategorical(int size, std::function<std::pair<T,double>(int)> init) {
        mca.reserve(size);
        categoryLabels.reserve(size);
        for(int i=size-1; i>=0; --i) {
            std::pair<T,double> v = init(i);
            mca.push_back(v.second);
            categoryLabels.push_back(std::move(v.first));
        }
    }


    void add(const T &categoryLabel, double weight);
    void add(T &&categoryLabel, double weight);
    void erase(iterator category);
    void set(iterator category, double weight);
    double weight(const_iterator category) const { return mca[category - categoryLabels.begin()]; }
    double probability(const_iterator category) const { return weight(category)/sum(); }
    double sum() const { return mca.sum(); }
    iterator begin() { return categoryLabels.begin(); }
    iterator end()   { return categoryLabels.end(); }
    iterator begin() const { return categoryLabels.begin(); }
    iterator end()   const { return categoryLabels.end(); }
    int size() const { return categoryLabels.size(); }
    template<class RNG> iterator operator()(RNG &randomGenerator) {
        return categoryLabels.begin() + mca(randomGenerator);
    }
    template<class RNG> const_iterator operator()(RNG &randomGenerator) const {
        return categoryLabels.begin() + mca(randomGenerator);
    }


    friend std::ostream &operator <<(std::ostream &out, const MutableCategorical<T> &distribution) {
        for(int i=0; i<distribution.size(); ++i) {
            out << distribution.categoryLabels[i] << " -> " << distribution.mca[i] << std::endl;
        }
        return out;
    }
};

// invalidates the iterator to the last element, iterator to the erased element
// becomes iterator to what was the last element...
template<class T>
void MutableCategorical<T>::erase(typename std::vector<T>::iterator category) {
    int categoryIndex = category - categoryLabels.begin();
    int lastCategoryIndex = size() - 1;
    if(categoryIndex != lastCategoryIndex) {
        mca.set(categoryIndex, mca[lastCategoryIndex]);
        categoryLabels[categoryIndex] = std::move(categoryLabels[lastCategoryIndex]);
    }
    mca.pop_back();
    categoryLabels.pop_back();
}

template<class T>
void MutableCategorical<T>::add(const T &categoryLabel, double weight) {
    mca.push_back(weight);
    categoryLabels.push_back(categoryLabel);
}

template<class T>
void MutableCategorical<T>::add(T &&categoryLabel, double weight) {
    mca.push_back(weight);
    categoryLabels.push_back(std::move(categoryLabel));
}

template<class T>
void MutableCategorical<T>::set(iterator category, double weight) {
    mca.set(category - categoryLabels.begin(), weight);
}


#endif //CPP_MUTABLECATEGORICAL_H
