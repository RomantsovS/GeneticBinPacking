cmake_minimum_required(VERSION 3.11)
project(GA VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    set(MY_COMLILE_OPTIONS /W4 /WX)
else ()
    set(MY_COMLILE_OPTIONS -Wall -Wextra -pedantic -Werror -Wshadow -fno-omit-frame-pointer)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(MY_COMLILE_OPTIONS ${MY_COMLILE_OPTIONS} -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC)
    endif()
endif()

include(CTest)
enable_testing()

add_executable(ga main.cpp GA.cpp)
target_compile_options(ga PRIVATE ${MY_COMLILE_OPTIONS})