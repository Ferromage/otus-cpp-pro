cmake_minimum_required(VERSION 3.15)

set(VERSION_PATCH "1" CACHE INTERNAL "Patch version")
project(mapreduce VERSION 0.0.${VERSION_PATCH})

add_executable(mapper mapper.cpp)
add_executable(reducer_avg reducer_avg.cpp)
add_executable(reducer_disp reducer_disp.cpp)

set_target_properties(mapper reducer_avg reducer_disp PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(mapper PRIVATE /W4)
    target_compile_options(reducer_avg PRIVATE /W4)
    target_compile_options(reducer_disp PRIVATE /W4)
else ()
    #target_compile_options(mapper PRIVATE -Wall -Wextra -pedantic -Werror)
    #target_compile_options(reducer_avg PRIVATE -Wall -Wextra -pedantic -Werror)
    #target_compile_options(reducer_disp PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

install(TARGETS mapper reducer_avg reducer_disp RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)
