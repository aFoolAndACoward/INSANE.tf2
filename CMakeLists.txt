cmake_minimum_required(VERSION 3.20)
project(Insane_tf2 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.cxx")
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    FILES ${SOURCES}
)


add_library(Insane_tf2 SHARED ${SOURCES})


# Yes, it must be msvc, cause I am not smart enough to make it work with clang-cl or gcc. ( I tried. )
if(NOT MSVC)
  message(FATAL_ERROR "This project must be built with MSVC (cl.exe).")
endif()


#//////////////////////////////////////////////////
# Linking DirectX9 library. ( dx9 SDK required. Download it and fix absolute address here. )
#//////////////////////////////////////////////////
target_link_libraries(Insane_tf2 PRIVATE d3d9 d3dx9)
target_include_directories(Insane_tf2 PRIVATE
    "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include"
)
target_link_directories(Insane_tf2 PRIVATE
    "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64"
)


#//////////////////////////////////////////////////
# Linking MinHook.lib x64 
#//////////////////////////////////////////////////
target_link_libraries(Insane_tf2 PRIVATE "${CMAKE_SOURCE_DIR}/includes/MinHook.lib")
# target_link_libraries(Insane_tf2 PRIVATE "${CMAKE_SOURCE_DIR}/src/Utility/Profiler")
# NOTE : CMAKE_SOURCE_DIR -> Where CMakeLists.txt is.
# NOTE : CMAKE_BINARY_DIR -> Where build files are.


file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/Build")
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/Output")
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/Output/Debug")
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/Output/Release")

set_target_properties(Insane_tf2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Output/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Output/Release"
)

target_compile_options(Insane_tf2 PRIVATE /SUBSYSTEM:WINDOWS )
