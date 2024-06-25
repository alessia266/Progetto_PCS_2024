list(APPEND src_sources "${CMAKE_CURRENT_SOURCE_DIR}/Utils.cpp")
list(APPEND src_headers "${CMAKE_CURRENT_SOURCE_DIR}/Utils.hpp")
list(APPEND src_headers "${CMAKE_CURRENT_SOURCE_DIR}/DFN.hpp")


set(src_sources ${src_sources} PARENT_SCOPE)
set(src_headers ${src_headers} PARENT_SCOPE)
set(src_includes ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

list(APPEND sorting_headers ${CMAKE_CURRENT_SOURCE_DIR}/SortingAlgorithm.hpp)
list(APPEND src_headers ${CMAKE_CURRENT_SOURCE_DIR}/DFN_Test.hpp)

list(APPEND sorting_includes ${CMAKE_CURRENT_SOURCE_DIR})


set(sorting_sources ${sorting_sources} PARENT_SCOPE)
set(sorting_headers ${sorting_headers} PARENT_SCOPE)
set(sorting_includes ${sorting_includes} PARENT_SCOPE)
