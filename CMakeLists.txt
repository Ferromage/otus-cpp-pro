cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(sfinae VERSION 0.0.${VERSION_PATCH})

add_executable(print_ip main.cpp)

set_target_properties(print_ip PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(print_ip PRIVATE /W4)    
else ()
    #target_compile_options(print_ip PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

install(TARGETS print_ip RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)