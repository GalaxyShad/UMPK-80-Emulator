cmake_minimum_required(VERSION 3.16)
project(umpk-80-emu-ui LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.90.4-docking)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)

set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(ImGui-SFML)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(umpk-80-emu-ui ${SOURCES})



target_link_libraries(umpk-80-emu-ui PRIVATE sfml-audio sfml-graphics ImGui-SFML::ImGui-SFML)
target_compile_features(umpk-80-emu-ui PRIVATE cxx_std_11)

target_compile_definitions(umpk-80-emu-ui
    PRIVATE
        # If the debug configuration pass the DEBUG define to the compiler 
        $<$<CONFIG:Debug>:DEBUG>
)

if(WIN32)
    add_custom_command(
        TARGET umpk-80-emu-ui
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:umpk-80-emu-ui>
        VERBATIM)
endif()

add_custom_command(
    TARGET umpk-80-emu-ui
    COMMENT "Copy data directory"
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/data 
        $<TARGET_FILE_DIR:umpk-80-emu-ui>/data 
    VERBATIM)

install(TARGETS umpk-80-emu-ui)