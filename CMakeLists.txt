cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(bulk VERSION 0.0.${VERSION_PATCH})

option(WITH_TEST "Whether to build test" OFF)

add_executable(bulk main.cpp parser.cpp printer.cpp)
set_target_properties(bulk PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

if(WITH_TEST)
    include(FetchContent)
    FetchContent_Declare(
        googletest
            URL https://github.com/google/googletest/archive/2f83a6e8f8aa56335f8b6a4d72c0a6841dcfa985.zip
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(test_parser test_parser.cpp parser.cpp)
    set_target_properties(test_parser PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
    target_link_libraries(test_parser GTest::gtest_main)
endif()

if (MSVC)
    target_compile_options(bulk PRIVATE /W4)

    if(WITH_TEST)
        target_compile_options(test_parser PRIVATE /W4)
    endif()
else ()
    target_compile_options(bulk PRIVATE -Wall -Wextra -pedantic -Werror)
    
    if(WITH_TEST)
        target_compile_options(test_parser PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

install(TARGETS bulk RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_TEST)
    enable_testing()
    add_test(test_parser test_parser)
endif()
