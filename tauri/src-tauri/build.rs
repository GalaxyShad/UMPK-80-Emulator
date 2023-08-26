
fn main() {
  cc::Build::new()
      .cpp(true)
      .include("../../src")
      .file("../../src/cpu.cpp")
      .file("../../src/cumpk80-external.cpp")
      .compile("cumpk80");

  tauri_build::build()
}
