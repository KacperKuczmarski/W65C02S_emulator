cmake_minimum_required(VERSION 4.0)
project(w65c02s LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

option(W65C02S_BUILD_TESTS "Build unit and functional tests" ON)
option(W65C02S_SANITIZE "Build with AddressSanitizer and UndefinedBehaviourSanitizer" OFF)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_library(w65c02s_options INTERFACE)
target_compile_features(w65c02s_options INTERFACE cxx_std_20)
target_compile_options(w65c02s_options INTERFACE -Wall -Wextra -Wpedantic -Wshadow -Werror)
if(W65C02S_SANITIZE)
    target_compile_options(w65c02s_options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(w65c02s_options INTERFACE -fsanitize=address,undefined)
endif ()

add_library(w65c02s src/CPU.cpp)
target_link_libraries(w65c02s PUBLIC w65c02s_options)

add_executable(w65c02s_cli main.cpp)
set_target_properties(w65c02s_cli PROPERTIES OUTPUT_NAME w65c02s)
target_link_libraries(w65c02s_cli PRIVATE w65c02s)

install(TARGETS w65c02s_cli RUNTIME DESTINATION bin)

if(W65C02S_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()