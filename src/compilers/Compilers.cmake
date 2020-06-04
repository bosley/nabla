
#
#   NHLL
#
#set(NHLL_COMPILER_DIR
#  ${NABLA_COMPILERS_DIR}/nhll
#)

set(DEL_COMPILER_DIR
  ${NABLA_COMPILERS_DIR}/del
)


#include(${NHLL_COMPILER_DIR}/nhll.cmake)

include(${DEL_COMPILER_DIR}/del.cmake)

#
#   Build the includes
#
set(NABLA_COMPILERS_INCLUDE_DIRS
    ${NABLA_COMPILERS_DIR}
    ${DEL_COMPILER_DIR}
    ${DEL_INCLUDE_DIRS}

   # ${NHLL_COMPILER_DIR}
   # ${NHLL_INCLUDE_DIRS}
)

#
#   Add sources for all compilers 
#
set(NABLA_COMPILERS_SOURCES
 #   ${NHLL_COMPILER_SOURCES}
    ${DEL_COMPILER_SOURCES}
)