cmake_minimum_required(VERSION 3.8)

project(NablaApp)

option(COMPILE_TESTS        "Execute compile-time tests"     ON)

include(${CMAKE_SOURCE_DIR}/cmake/FindCppuTest.cmake)

#-------------------------------------------------
#   Nabla Version Information
#-------------------------------------------------

include(${CMAKE_SOURCE_DIR}/cmake/NablaVersion.cmake)

#-------------------------------------------------
#   Platform Information
#-------------------------------------------------

include(${CMAKE_SOURCE_DIR}/cmake/Platform.cmake)

set(CMAKE_CXX_STANDARD 17)

find_package(libnabla REQUIRED)

find_package(Threads)

include_directories(${LIBNABLA_INCLUDE_DIRS})

#-------------------------------------------------
#   Compilers
#-------------------------------------------------

set(NABLA_COMPILERS_DIR 
    ${CMAKE_CURRENT_SOURCE_DIR}/compilers
)

include(${NABLA_COMPILERS_DIR}/Compilers.cmake)

include_directories(${NABLA_COMPILERS_INCLUDE_DIRS})

set(NABLA_APP_SOURCES
    ${LIBNABLA_LIBRARIES}
)

add_executable(nabla
        main.cpp
        ${NABLA_COMPILERS_SOURCES}
)

target_link_libraries(nabla
    PRIVATE 
        ${NABLA_APP_SOURCES}
        Threads::Threads
)
