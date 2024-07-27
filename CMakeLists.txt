cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(join_server VERSION 0.0.${VERSION_PATCH})

find_package(SQLite3 REQUIRED)

add_executable(join_server main.cpp server.cpp custom_database.cpp sqlite_database.cpp)
target_link_libraries(join_server SQLite::SQLite3)
set_target_properties(join_server PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(join_server PRIVATE /W4)
else ()
    target_compile_options(join_server PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

install(TARGETS join_server RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)
