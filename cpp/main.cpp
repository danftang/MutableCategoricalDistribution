#include <iostream>
#include <list>
#include <map>

#include "MutableCategoricalArray.h"
#include "TestMutableCategoricalArray.h"
#include "MutableCategorical.h"
#include "TestMutableCategorical.h"

int main() {
//    std::cout << "Starting MutableCategoricalArray test" << std::endl;
//    TestMutableCategoricalArray arrayTest;
//    arrayTest.doTest();

    std::cout << std::endl << "Starting MutableCategorical test" << std::endl;
    TestMutableCategorical catTest;
    catTest.doTest();

    return 0;
}
