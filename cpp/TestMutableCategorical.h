//
// Created by daniel on 03/08/22.
//

#ifndef CPP_TESTMUTABLECATEGORICAL_H
#define CPP_TESTMUTABLECATEGORICAL_H

#include <map>
#include <random>
#include <iostream>
#include <algorithm>
#include "MutableCategorical.h"


class TestMutableCategorical {
public:
    MutableCategorical<int> distriburion;
    std::map<int,double>    reference;
    std::vector<MutableCategorical<int>::iterator> categories;
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
            auto cat = distriburion.add(i, weight);
            categories.push_back(cat);
            assert(haveEqualEntries(reference, distriburion));
            if(i<100) assert(randomDrawIsCorrect(distriburion));
        }
        std::cout << "Successfully created distribution" << std::endl;
    }


    void testModification() {
        std::shuffle(categories.begin(), categories.end(), randomSource);
        for(auto it : categories) {
            double newWeight = std::uniform_real_distribution<double>()(randomSource);
            it->setWeight(newWeight);
            reference[it->value] = newWeight;
            assert(haveEqualEntries(reference, distriburion));
        }
        std::cout << "Successfully modified distribution" << std::endl;
    }

    void testDeletion() {
        std::shuffle(categories.begin(), categories.end(), randomSource);
        for(auto it : categories) {
            distriburion.erase(it);
            reference.erase(it->value);
            assert(haveEqualEntries(reference, distriburion));
            if(distriburion.size() < 100) assert(randomDrawIsCorrect(distriburion));
        }
        std::cout << "Successfully deleted all categories" << std::endl;
    }

    bool haveEqualEntries(std::map<int,double> &map, MutableCategorical<int> &distribution) {
        if(map.size() != distribution.size()) return false;
        int n = distribution.size();
        double sum = 0.0;
        for(const MutableCategorical<int>::Category &cat : distribution) {
            --n;
            auto mapEntry = map.find(cat.value);
            if(mapEntry == map.end() || cat.getWeight() != mapEntry->second) return false;
            sum += cat.getWeight();
        }
        if(n != 0) return false; // check distribution.size() is number of categories
        if(fabs(sum - distribution.sum()) > 1e-8) return false;
        return true;
    }

    bool randomDrawIsCorrect(MutableCategorical<int> &distribution) {
        if(distribution.size() == 0) return distribution.choose() == distribution.end();
        std::map<int,int> count;
        int nDraws = 100000;//distribution.size() * 500;
        for(auto entry: distribution) count[entry.value] = 0;
        for(int draw = 0; draw < nDraws; ++draw) {
            MutableCategorical<int>::iterator cat = distribution.choose();
            ++count[cat->value];
        }

        // calculate Pearson's chi squared in order to see if we can
        // discount the hypothesis that the samples came from the expected
        // distribution
        double chiSq = 0.0;
        for(auto entry: distribution) {
            double p = entry.getWeight()/distribution.sum();
            double expectedCount = p * nDraws;
            double sampleError = count[entry.value] - expectedCount;
            double sampleErrorSq = sampleError*sampleError;
            if(sampleErrorSq > 0.0) chiSq += sampleErrorSq / expectedCount; // deal correctly with case p=0
        }

        // use this to avoid requiring any external libraries
        std::chi_squared_distribution<double> chiSqDist(distribution.size() - 1.0);
        int nDrawsAtLeastAsBadAsSample = 0;
        const int nChiSqDraws = 100000;
        for(int i=0; i<nChiSqDraws; ++i) {
            if(chiSqDist(randomSource) >= chiSq) ++nDrawsAtLeastAsBadAsSample;
        }
        double pValue = nDrawsAtLeastAsBadAsSample*1.0/nChiSqDraws;
        std::cout << "Proportion at least as bad as sample = " << pValue << std::endl;
        if(pValue < 0.0001) {
            for(auto entry: distribution) {
                std::cout << nDraws*entry.getWeight()/distribution.sum() << " " << count[entry.value] << std::endl;
            }
            return false;
        }
        return true;
    }

};


#endif //CPP_TESTMUTABLECATEGORICAL_H
