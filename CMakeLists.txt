cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

set(SRC_DIR "./src")
project(editor VERSION ${PROJECT_VESRION})

# option(WITH_BOOST_TEST "Whether to build Boost test" ON)

add_library(editor INTERFACE)

target_include_directories(editor INTERFACE
    ${SRC_DIR}
)

add_executable(editor_cli
    ${SRC_DIR}/main.cpp
)

set_target_properties(editor_cli PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(editor_cli PRIVATE editor)

# if(WITH_BOOST_TEST)
#     find_package(Boost COMPONENTS unit_test_framework REQUIRED)

#     add_executable(test_version
#         test_version.cpp
#     )

#     set_target_properties(test_version PROPERTIES
#         CXX_STANDARD 14
#         CXX_STANDARD_REQUIRED ON
#         COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
#         INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
#     )

#     target_link_libraries(test_version
#         ${Boost_LIBRARIES}
#         editor
#     )
# endif()

if (MSVC)
    target_compile_options(editor_cli PRIVATE /W4)
    # if(WITH_BOOST_TEST)
    #     target_compile_options(test_version PRIVATE /W4)
    # endif()
else ()
    target_compile_options(editor_cli PRIVATE -Wall -Wextra -pedantic -Werror)
    # if(WITH_BOOST_TEST)
    #     target_compile_options(test_version PRIVATE -Wall -Wextra -pedantic -Werror)
    # endif()
endif()

install(TARGETS editor_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)