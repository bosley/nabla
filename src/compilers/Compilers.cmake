
#
#   Build the includes
#
set(NABLA_COMPILERS_INCLUDE_DIRS
    ${NABLA_COMPILERS_DIR}
    ${NABLA_COMPILERS_DIR}/nhll
)

#
#   Build the sources for the NHLL compiler
#
set(NABLA_COMPILER_NHLL
    ${NABLA_COMPILERS_DIR}/nhll/NhllVisitor.cpp
    ${NABLA_COMPILERS_DIR}/nhll/NhllParser.cpp
    ${NABLA_COMPILERS_DIR}/nhll/NhllCompiler.cpp
)

#
#   Add sources for all compilers 
#
set(NABLA_COMPILERS_SOURCES
    ${NABLA_COMPILER_NHLL}

    ${NABLA_COMPILERS_DIR}/CompiledPackage.cpp
)

#-------------------------------------------------
# Tests
#-------------------------------------------------

if(COMPILE_TESTS)
        add_subdirectory(${NABLA_COMPILERS_DIR}/nhll/tests)
endif()