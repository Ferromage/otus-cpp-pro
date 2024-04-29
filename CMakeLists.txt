cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(ip_filter VERSION 0.0.${VERSION_PATCH})

option(WITH_TEST "Whether to build test" ON)

add_executable(ip_filter ip_filter.cpp util.cpp)
set_target_properties(ip_filter PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if(WITH_TEST)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/2f83a6e8f8aa56335f8b6a4d72c0a6841dcfa985.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(ip_filter_test ip_filter_test.cpp util.cpp)    
    target_link_libraries(ip_filter_test GTest::gtest_main)
    set_target_properties(ip_filter_test PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
endif()

if (MSVC)
    target_compile_options(ip_filter PRIVATE /W4)    

    if(WITH_TEST)
        target_compile_options(ip_filter_test PRIVATE /W4)
    endif()
else ()
    target_compile_options(ip_filter PRIVATE -Wall -Wextra -pedantic -Werror)    
    
    if(WITH_TEST)
        target_compile_options(ip_filter_test PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

install(TARGETS ip_filter RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_TEST)
    enable_testing()
    add_test(ip_filter_test ip_filter_test)
endif()