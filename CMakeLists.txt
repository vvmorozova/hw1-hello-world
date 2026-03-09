cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(editor VERSION ${PROJECT_VERSION})

set(SRC_DIR "./src")
set(INC_DIR "./inc")
set(TESTS_DIR "./tests")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")

list(FILTER SRC_FILES EXCLUDE REGEX ".*_test\\.cpp$")

add_executable(matrix_cli ${SRC_FILES})
target_include_directories(matrix_cli PRIVATE ${INC_DIR})

set_target_properties(matrix_cli PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if (MSVC)
    target_compile_options(matrix_cli PRIVATE /W4)
else()
    target_compile_options(matrix_cli PRIVATE -Wall -Wextra -pedantic -Werror -g)
endif()

install(TARGETS matrix_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
include(CPack)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
add_subdirectory(./googletest)

file(GLOB TEST_FILES "${TESTS_DIR}/tests.cpp")

add_executable(matrix_test ${TEST_FILES})
target_include_directories(matrix_test PRIVATE ${INC_DIR})

target_link_libraries(matrix_test PRIVATE GTest::gtest_main)

if (MSVC)
    target_compile_options(matrix_test PRIVATE /W4)
else()
    target_compile_options(matrix_test PRIVATE -Wall -Wextra -Werror -pedantic -g)
endif()

include(GoogleTest)
gtest_discover_tests(matrix_test)