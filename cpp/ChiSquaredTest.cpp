//
// Created by daniel on 05/08/22.
//

#include <random>
#include <iostream>

// returns true if the p-value associated with the given chiSquared
// value is less than the supplied pValue.
// Uses Monte-carlo integration to avoid requiring any external libraries,
// this is fine since we're only looking for obvious discrepancies.
bool pValueIsLessThan(double chiSquared, int nDegreesOfFreedom, double pValue) {
    static std::mt19937 randomSource;

    std::chi_squared_distribution<double> chiSqDist(nDegreesOfFreedom);
    const int maxSamplesAsBadAsMeasurement = 10; // as soon as we draw this many worse samples we're satisfied
    const int maxSamples = maxSamplesAsBadAsMeasurement / pValue;
    int nSamplesAsBadAsMeasurement = 0;
    int nSamples = 0;
    while(nSamplesAsBadAsMeasurement < maxSamplesAsBadAsMeasurement && nSamples <= maxSamples) {
        if(chiSqDist(randomSource) >= chiSquared) ++nSamplesAsBadAsMeasurement;
        ++nSamples;
    }
    std::cout << "chiSq = " << chiSquared << " k = " << nDegreesOfFreedom << " p-value = " << nSamplesAsBadAsMeasurement * 1.0 / nSamples << std::endl;
    return (nSamples > maxSamples);
}