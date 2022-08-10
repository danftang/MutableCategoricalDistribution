//
// Created by daniel on 03/08/22.
//

#ifndef CPP_TESTMUTABLECATEGORICAL_H
#define CPP_TESTMUTABLECATEGORICAL_H

#include <map>
#include <random>
#include <iostream>
#include <algorithm>
#include "ChiSquaredTest.h"

// DIST is the mutable categorical class to test, categories should be associated with integers
// and DIST should implement add, erase, operator (), probability(), begin(), end() which iterate over
// integers
template<class DIST>
class TestMutableCategorical {
public:
    DIST                    distribution;
    std::map<int,double>    reference;
    const int               nInitCategories = 1000;
    std::mt19937            randomSource;


    void doTest() {
        testCreation();
        testModification();
        testDeletion();
    }


    void testCreation() {
        for(int i=0; i < nInitCategories; ++i) {
            double weight = std::uniform_real_distribution<double>()(randomSource);
            reference[i] = weight;
            distribution.add(i, weight);
            assert(haveEqualEntries(reference, distribution));
            if(i<100) assert(randomDrawIsCorrect(distribution));
        }
        std::cout << "Successfully created distribution" << std::endl;
    }


    void testModification() {
        std::vector<typename DIST::iterator> categories;
        for(auto it = distribution.begin(); it != distribution.end(); ++it) categories.push_back(it);
        std::shuffle(categories.begin(), categories.end(), randomSource);
        for(auto it : categories) {
            double newWeight = std::uniform_real_distribution<double>()(randomSource);
            distribution.set(it, newWeight);
            reference[*it] = newWeight;
            assert(haveEqualEntries(reference, distribution));
        }
        std::cout << "Successfully modified distribution" << std::endl;
    }


    void testDeletion() {
        while(distribution.size() > 0) {
            auto it = distribution(randomSource);
            reference.erase(*it);
            distribution.erase(it);
            assert(haveEqualEntries(reference, distribution));
            if(distribution.size() < 100) assert(randomDrawIsCorrect(distribution));
        }
        std::cout << "Successfully deleted all categories" << std::endl;
    }


    bool haveEqualEntries(std::map<int,double> &map, DIST &distribution) {
        if(map.size() != distribution.size()) return false;
        int n = distribution.size();
        double sum = 0.0;
        for(auto cat = distribution.begin(); cat != distribution.end(); ++cat) {
            --n;
            auto mapEntry = map.find(*cat);
            if(mapEntry == map.end()) {
                std::cout << "Unexpected category present in distribution" << std::endl;
                return false;
            }
            if(fabs(distribution.weight(cat) - mapEntry->second) > 1e-8) {
                std::cout << "Incorrect weight on category " << mapEntry->first
                        << " expected " << mapEntry->second
                        << " but found " << distribution.weight(cat)
                        << std::endl;
                return false;
            }
            sum += distribution.weight(cat);
        }
        if(n != 0) return false; // check distribution.size() is number of categories
        if(fabs(sum - distribution.sum()) > 1e-8) return false;
        return true;
    }


    // returns true if the p-value that 100000 draws is greater than 0.0001
    // i.e. we can't discount the hypothesis that the draws came from the
    // correct distribution.
    bool randomDrawIsCorrect(DIST &distribution) {
        if(distribution.size() == 0) return distribution(randomSource) == distribution.end();
        std::map<int,int> count;
        int nDraws = 100000;
        for(auto entry: distribution) count[entry] = 0;
        for(int draw = 0; draw < nDraws; ++draw) {
            auto catIt = distribution(randomSource);
            ++count[*catIt];
        }

        // calculate Pearson's chi squared in order to calculate p-value of the
        // hypothesis that the draws came from the correct distribution
        double chiSq = 0.0;
        for(auto it = distribution.begin(); it != distribution.end(); ++it) {
            double p = distribution.probability(it);
            double expectedCount = p * nDraws;
            double sampleError = count[*it] - expectedCount;
            double sampleErrorSq = sampleError*sampleError;
            if(sampleErrorSq > 0.0) chiSq += sampleErrorSq / expectedCount; // deal correctly with case p=0
        }
        return !pValueIsLessThan(chiSq, distribution.size()-1, 0.0001);
    }

};


#endif //CPP_TESTMUTABLECATEGORICAL_H
