// MyLibraryProject.cpp : Source file for your target.
//

#include "MyLibraryProject.h"
#include <iostream>

extern "C" {
    MYLIB_API void sayHello() {
        std::cout << "Hello from DLL!" << std::endl;
    }
}