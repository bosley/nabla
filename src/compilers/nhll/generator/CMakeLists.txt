set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated-register")

project(nhll_generator CXX)

cmake_minimum_required(VERSION 3.1)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(nhll_parser
             nhll_parser.yy
             ${CMAKE_CURRENT_BINARY_DIR}/nhll_parser.tab.cc)
FLEX_TARGET(nhll_lexer
            nhll_lexer.l
            ${CMAKE_CURRENT_BINARY_DIR}/nhll_lexer.yy.cc)
ADD_FLEX_BISON_DEPENDENCY(nhll_lexer nhll_parser)

add_executable(my_wc
    main.cpp
    nhll_driver.cpp

    ${FLEX_nhll_lexer_OUTPUTS}
    ${BISON_nhll_parser_OUTPUTS}
)
target_include_directories(my_wc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${FLEX_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR})
set_property(TARGET my_wc
             PROPERTY CXX_STANDARD 14)

enable_testing()
add_test(NAME basic
         COMMAND ${CMAKE_SOURCE_DIR}/test/test0.pl
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
