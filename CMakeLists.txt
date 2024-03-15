cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(helloworld VERSION 0.0.${VERSION_PATCH})

option(WITH_TEST "Whether to build test" ON)

configure_file(version.h.in version.h)

add_executable(helloworld_cli main.cpp)
add_library(hello_lib lib.cpp)

set_target_properties(helloworld_cli hello_lib PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories(hello_lib PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(helloworld_cli PRIVATE hello_lib)

if(WITH_TEST)
    include(FetchContent)
    FetchContent_Declare(
        googletest
            URL https://github.com/google/googletest/archive/2f83a6e8f8aa56335f8b6a4d72c0a6841dcfa985.zip
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(test_version test_version.cpp)

    set_target_properties(test_version PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )    

    target_link_libraries(test_version
        GTest::gtest_main
        hello_lib
    )
endif()

if (MSVC)
    target_compile_options(helloworld_cli PRIVATE /W4)
    target_compile_options(hello_lib PRIVATE /W4)

    if(WITH_TEST)
        target_compile_options(test_version PRIVATE /W4)
    endif()
else ()
    target_compile_options(helloworld_cli PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(hello_lib PRIVATE -Wall -Wextra -pedantic -Werror)    
    
    if(WITH_TEST)
        target_compile_options(test_version PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

install(TARGETS helloworld_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_TEST)
    enable_testing()
    add_test(test_version test_version)
endif()