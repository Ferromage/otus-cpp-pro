cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(client VERSION 0.0.${VERSION_PATCH})

add_executable(client main.cpp client_controller.cpp tcp_client.cpp)

set_target_properties(client PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories(client PRIVATE ${CMAKE_BINARY_DIR})

if (MSVC)
    target_compile_options(client PRIVATE /W4)
else ()
    #target_compile_options(client PRIVATE -Wall -Wextra -pedantic -Werror)
endif()