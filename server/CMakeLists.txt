cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(server VERSION 0.0.${VERSION_PATCH})

#find_package(Boost REQUIRED asio)
add_executable(server main.cpp tcp_server.cpp user_controller.cpp)

set_target_properties(server PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
    #target_compile_options(server PRIVATE /W4)
else ()
    #target_compile_options(server PRIVATE -Wall -Wextra -pedantic -Werror)
endif()