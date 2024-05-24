cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(bulk VERSION 0.0.${VERSION_PATCH})

add_executable(bulk main.cpp)
set_target_properties(bulk PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(bulk PRIVATE /W4)
else ()
    target_compile_options(bulk PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

install(TARGETS bulk RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)
