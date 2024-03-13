cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(helloworld VERSION 0.0.${VERSION_PATCH})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

configure_file(version.h.in version.h)

add_executable(hello main.cpp)
add_library(hello_lib lib.cpp)

set_target_properties(hello hello_lib PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories(hello_lib PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(hello PRIVATE hello_lib)

if(WITH_BOOST_TEST)
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)
    add_executable(test_version test_version.cpp)

    set_target_properties(test_version PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
    )

    set_target_properties(test_version PROPERTIES
        COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
        INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
    )

    target_link_libraries(test_version
        ${Boost_LIBRARIES}
        hello_lib
    )
endif()

if (MSVC)
    target_compile_options(hello PRIVATE /W4)
    target_compile_options(hello_lib PRIVATE /W4)

    if(WITH_BOOST_TEST)
        target_compile_options(test_version PRIVATE /W4)
    endif()
else ()
    target_compile_options(hello PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(hello_lib PRIVATE -Wall -Wextra -pedantic -Werror)    
    
    if(WITH_BOOST_TEST)
        target_compile_options(test_version PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

install(TARGETS hello RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_BOOST_TEST)
    enable_testing()
    add_test(test_version test_version)
endif()