fn main() {
    cc::Build::new()
        .cpp(true)
        .include("../src")
        .file("../src/cpu.cpp")
        .file("../src/cumpk80-external.cpp")
        
        // .file("../src/cumpk80-external.hpp")
        // .file("../src/bus.hpp")
        // .file("../src/cpu.hpp")
        // .file("../src/cumpk80-external.hpp")
        // .file("../src/disassembler.hpp")
        // .file("../src/display.hpp")
        // .file("../src/keyboard.hpp")
        // .file("../src/register.hpp")
        // .file("../src/umpk80.hpp")
        .compile("cumpk80");
}