link_directories(${CPPUTEST_LIBRARIES})

include_directories(${NABLA_COMPILERS_INCLUDE_DIRS})

add_executable(nhllTests
        ${NABLA_COMPILERS_SOURCES}
        parserTests.cpp
        main.cpp
)


target_link_libraries(nhllTests 
                      ${CPPUTEST_LDFLAGS}
                      Threads::Threads
)

add_custom_command(TARGET nhllTests COMMAND ./nhllTests POST_BUILD)