cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(ip_filter VERSION 0.0.${VERSION_PATCH})

option(WITH_TEST "Whether to build test" ON)

add_executable(ip_filter ip_filter.cpp)
set_target_properties(ip_filter PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

#if(WITH_TEST)
#endif()

if (MSVC)
    target_compile_options(ip_filter PRIVATE /W4)    

    if(WITH_TEST)
        #target_compile_options(test_version PRIVATE /W4)
    endif()
else ()
    target_compile_options(ip_filter PRIVATE -Wall -Wextra -pedantic -Werror)    
    
    if(WITH_TEST)
        #target_compile_options(test_version PRIVATE -Wall -Wextra -pedantic -Werror)
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
    #enable_testing()
    #add_test(test_version test_version)
endif()