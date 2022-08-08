#include <iostream>
#include <list>
#include <map>

#include "MutableCategoricalArray.h"
#include "MutableCategorical.h"
#include "TestMutableCategoricalArray.h"
#include "MutableCategoricalTree.h"
#include "TestMutableCategorical.h"


int main() {
//    std::cout << "Starting MutableCategoricalArray test" << std::endl;
//    TestMutableCategoricalArray arrayTest;
//    arrayTest.doTest();

//    std::cout << std::endl << "Starting MutableCategorical test" << std::endl;
//    TestMutableCategorical catTest;
//    catTest.doTest();

    std::mt19937 rng;

    MutableCategorical<int> myP;
    myP.add(5,0.5);
    myP.add(7,0.5);
    std::cout << myP << std::endl;

//    MutableCategoricalArray myP2;
//    myP2.push_back(0.5);
//    myP2.push_back(0.5);
//    std::cout << myP2 << std::endl;

    for(int i=0; i<10; ++i) {
        std::cout << *myP(rng) << std::endl;
    }


    return 0;
}
