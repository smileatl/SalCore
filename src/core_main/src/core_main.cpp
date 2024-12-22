#include <base.h>
#include <iostream>

#include "core_main.h"

int main(int argc, char* argv[]) {
    sayHello();
    std::cout << "Hello SalCore!" << std::endl;
    base_apr_initialize();
    return 0;
}