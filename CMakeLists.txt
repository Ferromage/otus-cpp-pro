cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(messager VERSION 0.0.${VERSION_PATCH})

add_executable(client client/main.cpp)
add_executable(server server/main.cpp)

set_target_properties(client server PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(client PRIVATE /W4)
    target_compile_options(server PRIVATE /W4)
else ()
    target_compile_options(client PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(server PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

install(TARGETS client server RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)