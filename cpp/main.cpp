#include <iostream>

#include "MutableCategorical.h"
#include "test/TestMutableCategoricalArray.h"
#include "MutableCategoricalMap.h"
#include "test/TestMutableCategorical.h"

int main() {
    std::cout << "Starting MutableCategoricalArray test" << std::endl;
    TestMutableCategoricalArray arrayTest;
    arrayTest.doTest();

    std::cout << std::endl << "Starting MutableCategoricalMap test" << std::endl;
    TestMutableCategorical<MutableCategoricalMap<int>> treeTest;
    treeTest.doTest();

    std::cout << std::endl << "Starting MutableCategorical test" << std::endl;
    TestMutableCategorical<MutableCategorical<int>> catTest;
    catTest.doTest();

    return 0;
}
