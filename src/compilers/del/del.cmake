BISON_TARGET(del_parser
            ${DEL_COMPILER_DIR}/del_parser.yy
            ${CMAKE_CURRENT_BINARY_DIR}/del_parser.tab.cc)

FLEX_TARGET(del_lexer
            ${DEL_COMPILER_DIR}/del_lexer.l
            ${CMAKE_CURRENT_BINARY_DIR}/del_lexer.yy.cc)

ADD_FLEX_BISON_DEPENDENCY(del_lexer del_parser)

set(DEL_INCLUDE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEL_COMPILER_DIR}/delutil
    ${DEL_COMPILER_DIR}/codegen
    ${DEL_COMPILER_DIR}/codegen/asm
    ${FLEX_INCLUDE_DIRS}
)

set(DEL_COMPILER_HEADERS
    ${DEL_COMPILER_DIR}
    ${DEL_COMPILER_DIR}/delutil/Ast.hpp
    ${DEL_COMPILER_DIR}/delutil/Errors.hpp
    ${DEL_COMPILER_DIR}/delutil/SymbolTable.hpp
    ${DEL_COMPILER_DIR}/delutil/Memory.hpp
    ${DEL_COMPILER_DIR}/delutil/Tracker.hpp
    ${DEL_COMPILER_DIR}/codegen/Codegen.hpp
    ${DEL_COMPILER_DIR}/codegen/Analyzer.hpp
    ${DEL_COMPILER_DIR}/codegen/Intermediate.hpp
    ${DEL_COMPILER_DIR}/codegen/asm/AsmMath.hpp
    ${DEL_COMPILER_DIR}/codegen/asm/AsmSupport.hpp
    ${DEL_COMPILER_DIR}/del_driver.hpp
    ${DEL_COMPILER_DIR}/del_scanner.hpp
)

set(DEL_COMPILER_SOURCES
    ${DEL_COMPILER_DIR}/delutil/Ast.cpp
    ${DEL_COMPILER_DIR}/delutil/Errors.cpp
    ${DEL_COMPILER_DIR}/delutil/SymbolTable.cpp
    ${DEL_COMPILER_DIR}/delutil/Memory.cpp
    ${DEL_COMPILER_DIR}/delutil/Tracker.cpp
    ${DEL_COMPILER_DIR}/codegen/Codegen.cpp
    ${DEL_COMPILER_DIR}/codegen/Analyzer.cpp
    ${DEL_COMPILER_DIR}/codegen/Intermediate.cpp
    ${DEL_COMPILER_DIR}/codegen/asm/AsmSupport.cpp
    ${DEL_COMPILER_DIR}/del_driver.cpp
    ${FLEX_del_lexer_OUTPUTS}
    ${BISON_del_parser_OUTPUTS}
)

