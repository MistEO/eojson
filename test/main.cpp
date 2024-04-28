#include <iostream>

#include "include_test.h"
#include "json5_test.h"
#include "location_test.h"
#include "serializing_test.h"

int main()
{
    std::cout << "\n****** Testing ******\n" << std::endl;

    std::cout << "\n*** serializing ***\n" << std::endl;
    bool success = serializing();

    std::cout << "\n*** wstring_serializing ***\n" << std::endl;
    success &= wstring_serializing();

    std::cout << "\n*** jsonizing ***\n" << std::endl;
    success &= jsonizing();

    std::cout << "\n*** include_test ***\n" << std::endl;
    success &= include_test();

    std::cout << "\n*** json5_test ***\n" << std::endl;
    success &= test_json5();

    std::cout << "\n*** location_test ***\n" << std::endl;
    success &= location_test();

    if (!success) {
        std::cout << "\n****** Test failed ******\n" << std::endl;
        return -1;
    }

    std::cout << "\n****** Test passed ******\n" << std::endl;
    return 0;
}
