#include <iostream>

#include "MutableCategoricalArray.h"
#include "TestMutableCategoricalArray.h"

int main() {
    TestMutableCategoricalArray test;
    test.testOddCases();
    test.testTriangular();
    test.testModification();
    return 0;
}
