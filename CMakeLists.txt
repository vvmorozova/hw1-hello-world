cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(editor VERSION ${PROJECT_VERSION})

set(SRC_DIR "./src")
set(INC_DIR "./inc")

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")

add_executable(editor_cli ${SRC_FILES})

target_include_directories(editor_cli PRIVATE ${INC_DIR})

set_target_properties(editor_cli PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

if (MSVC)
    target_compile_options(editor_cli PRIVATE /W4)
else()
    target_compile_options(editor_cli PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

install(TARGETS editor_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)