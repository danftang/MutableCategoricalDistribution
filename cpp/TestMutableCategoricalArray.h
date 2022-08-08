#ifndef CPP_TESTMUTABLECATEGORICALARRAY_H
#define CPP_TESTMUTABLECATEGORICALARRAY_H


#include <assert.h>

#include "MutableCategoricalArray.h"
#include "ChiSquaredTest.h"

class TestMutableCategoricalArray {
public:
    std::default_random_engine rng;

    void doTest() {
        testOddCases();
        testInitialization();
        testTriangular();
        testModification();
    }

    void testOddCases() {
        //singleton
        MutableCategoricalArray dist1 {0.1};
        assert(dist1(rng) == 0);

        // zero probs
        MutableCategoricalArray dist2 {0.0, 0.0, 1.0, 0.0, 0.0, 0.0};
        assert(dist2(rng) == 2);
        std::cout << "Passed OddCases test" << std::endl;
    }

    void testInitialization() {
        MutableCategoricalArray myDistribution{0.4,0.6};
        assert(myDistribution[0] == 0.4);
        assert(myDistribution[1] == 0.6);
        std::cout << "Passed initialisation test" << std::endl;
    }
//
    void testTriangular() {
        int N = 10;
        MutableCategoricalArray triangularDistribution(N,[](int i) { return i; });
        testDistribution(triangularDistribution, 1000000);
        std::cout << "Passed Triangular distribution test" << std::endl;
    }

    void testModification() {
        int N = 5;
        std::uniform_real_distribution<double> uniformDist(0.0,1.0);
        std::uniform_int_distribution<int> indexDist(0,N-1);
        std::vector<double> targetDist(N);
        for(int i=0; i<N; ++i) {
            targetDist[i] = uniformDist(rng);
        }
        MutableCategoricalArray testDist(N, [&targetDist](int i){ return targetDist[i]; });
        for(int i=0; i<100; ++i) {
            testDistribution(testDist, targetDist, 1000000);
            int index = indexDist(rng);
            double newVal = uniformDist(rng);
            targetDist[index] = newVal;
            testDist[index] = newVal;
        }
        std::cout << "Passed Modification test" << std::endl;
    }

    void testDistribution(MutableCategoricalArray dist, int nSamples) {
        std::vector<int> histogram(dist.size(),0);
        for(int i=0; i<nSamples; ++i) {
            histogram[dist(rng)] += 1;
        }

        double chiSq = 0.0;
        for(int i=0; i<dist.size(); ++i) {
//            double sd = sqrt(nSamples*dist.P(i)*(1.0-dist.P(i)));
//            double sampleError = dist.P(i)*nSamples - histogram[i];
//            assert(fabs(sampleError) <= 4.0*sd);
            double expectedCount = dist.P(i) * nSamples;
            double sampleError = histogram[i] - expectedCount;
            double sampleErrorSq = sampleError*sampleError;
            if(sampleErrorSq > 0.0) chiSq += sampleErrorSq / expectedCount; // deal correctly with case p=0
        }

        assert(!pValueIsLessThan(chiSq, dist.size()-1, 0.0001));
    }

    void testDistribution(MutableCategoricalArray dist, std::vector<double> targetPMF, int nSamples) {
        assert(dist.size() == targetPMF.size());
        double targetSum = 0.0;
        for(int i=0; i<targetPMF.size(); ++i) targetSum += targetPMF[i];
        for(int i=0; i<targetPMF.size(); ++i) {
            assert(fabs(targetPMF[i]/targetSum - dist.P(i)) < 1e-15);
        }
        testDistribution(dist, nSamples);
    }

};

#endif
