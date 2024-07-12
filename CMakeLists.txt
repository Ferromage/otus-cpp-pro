cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(bulk_server VERSION 0.0.${VERSION_PATCH})

#find_package(Boost REQUIRED asio)
add_executable(bulk_server main.cpp server.cpp)
add_library(async SHARED async.cpp parser.cpp)
target_link_libraries(bulk_server async)
#target_link_libraries(bulk_server Boost::program_options)

set_target_properties(bulk_server async PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(bulk_server PRIVATE /W4)
    target_compile_options(async PRIVATE /W4)
else ()
    target_compile_options(bulk_server PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(async PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

install(TARGETS bulk_server RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)