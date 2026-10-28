cmake_minimum_required(VERSION 3.20)
project(GWBASIC_CPP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/basic_strings.cpp
    src/basic_memory.cpp
    src/basic_math.cpp
    src/basic_graphics.cpp
    src/basic_io.cpp
    src/basic_eval.cpp
    src/basic_interpreter.cpp
)

include_directories(src)

# ---- SDL2 Detection ----
find_package(SDL2 QUIET)

if(NOT SDL2_FOUND)
    # MinGW / MSYS2 manual paths
    set(SDL2_INCLUDE_DIRS "C:/msys64/mingw64/include/SDL2")
    set(SDL2_LIBRARIES    "-LC:/msys64/mingw64/lib -lSDL2main -lSDL2")
    set(SDL2_FOUND TRUE)
    message(STATUS "SDL2 detected via MSYS2 MinGW64 paths")
else()
    message(STATUS "SDL2 found via CMake: ${SDL2_LIBRARIES}")
endif()

# ---- Main Executable (CLI REPL + SDL2 Display) ----
add_executable(gwbasic
    src/main.cpp
    src/sdl_window.cpp
    ${SOURCES}
)
target_include_directories(gwbasic PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(gwbasic PRIVATE ${SDL2_LIBRARIES})
if(WIN32)
    # Link Windows subsystem as console (not GUI) so we keep stdout + SDL window
    set_target_properties(gwbasic PROPERTIES LINK_FLAGS "-mconsole")
endif()

# ---- Test Executables (no SDL2 dependency) ----
enable_testing()

add_executable(test_module1 tests/test_module1.cpp src/basic_strings.cpp src/basic_memory.cpp)
add_test(NAME TestModule1 COMMAND test_module1)

add_executable(test_module2 tests/test_module2.cpp src/basic_math.cpp)
add_test(NAME TestModule2 COMMAND test_module2)

add_executable(test_module3 tests/test_module3.cpp src/basic_graphics.cpp)
add_test(NAME TestModule3 COMMAND test_module3)

add_executable(test_module4 tests/test_module4.cpp ${SOURCES})
add_test(NAME TestModule4 COMMAND test_module4)

add_executable(test_module5 tests/test_module5.cpp ${SOURCES})
add_test(NAME TestModule5 COMMAND test_module5)

add_executable(test_advanced tests/test_advanced.cpp ${SOURCES})
add_test(NAME TestAdvanced COMMAND test_advanced)
