cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(cmd_parser VERSION 0.0.${VERSION_PATCH})

add_executable(cmd_parser main.cpp parser.cpp)
set_target_properties(cmd_parser PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(cmd_parser PRIVATE /W4)    
else ()
    target_compile_options(cmd_parser PRIVATE -Wall -Wextra -pedantic -Werror)    
endif()

install(TARGETS cmd_parser RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)