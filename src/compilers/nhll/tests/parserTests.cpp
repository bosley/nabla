
#include <iostream>
#include <random>

#include "NhllCompiler.hpp"

#include "CppUTest/TestHarness.h"

/*
    Language : 

    ';' Separates statements on same line. not required if only one

    Assignment 

        let VAR = something;


*/

TEST_GROUP(NHLLParserTests)
{   
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NHLLParserTests, allTests)
{
    NABLA::COMPILERS::Nhll nhllCompiler;

    NABLA::COMPILERS::NhllParser parser(nhllCompiler); // If we want to interp instead of compile, we can add the interp here instead!

/*
        Note: Because the compiler and the parser will be needing to generate blocks of instructions, and handle if variables exist I'd like to add something
        that they BOTH use that understands the operations of the VM to determine when registers should be used, and to handle blocks of bytes being generated

*/

    // Note: With the compiler, the actual use-case will be to hand it a file and it will call the parser. 
    parser.parseLine("let a = 3");
    parser.parseLine("let a = 4;");
    parser.parseLine("let a = 5; let a = 6");
}
