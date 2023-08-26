#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use tauri::Manager;
use tauri::Window;
use std::{thread, time};

use std::fs::File;
use std::io::Read;

#[link(name = "cumpk80")]
extern {
    pub fn hello() -> libc::c_int;

    pub fn UMPK80_Create() -> *mut libc::c_void;
    pub fn UMPK80_Free(umpk: *mut libc::c_void);

    pub fn UMPK80_PortIO_In_Set(umpk: *mut libc::c_void, data: u8);
    pub fn UMPK80_PortIO_In_Get(umpk: *mut libc::c_void) -> u8;

    pub fn UMPK80_PortIO_Out_Get(umpk: *mut libc::c_void) -> u8;

    pub fn UMPK80_Tick(umpk: *mut libc::c_void);
    pub fn UMPK80_Stop(umpk: *mut libc::c_void);
    pub fn UMPK80_Restart(umpk: *mut libc::c_void);

    pub fn UMPK80_PressKey(umpk: *mut libc::c_void);
    pub fn UMPK80_ReleaseKey(umpk: *mut libc::c_void);

    pub fn UMPK80_GetDisplayDigit(umpk: *mut libc::c_void, digit: libc::c_int) -> u8;
    pub fn UMPK80_LoadOS(umpk: *mut libc::c_void, os: *mut u8);

    pub fn UMPK80_Cpu_GetProgramCounter(umpk: *mut libc::c_void) -> u16;
}


// the payload type must implement `Serialize` and `Clone`.
#[derive(Clone, serde::Serialize)]
struct Payload {
  digit0: u8,
  digit1: u8,
  digit2: u8,
  digit3: u8,

  digit4: u8,
  digit5: u8,

  pg: u16
}

static mut digit0: u8 = 0;
static mut digit1: u8 = 0;
static mut digit2: u8 = 0;
static mut digit3: u8 = 0;

static mut digit4: u8 = 0;
static mut digit5: u8 = 0;

static mut pg: u16 = 0;

#[tauri::command]
async fn progress_tracker(window: Window) { 

  loop {

    unsafe {
      window.emit("PROGRESS", Payload { digit0, digit1, digit2, digit3, digit4, digit5, pg });
    }

    let delay = time::Duration::from_millis(1);
    thread::sleep(delay);
  }

}

fn main() {
  println!("Hello from Rust");

  thread::spawn(|| {
    unsafe {
      let mut file = File::open("../../data/scaned-os-fixed.bin")
        .expect("Failed to open file");

      let mut contents = Vec::new();
      file.read_to_end(&mut contents)
        .expect("Failed to read file");

      let umpk = UMPK80_Create();
      UMPK80_LoadOS(umpk, contents.as_mut_ptr());

      loop {
        UMPK80_Tick(umpk);

        pg = UMPK80_Cpu_GetProgramCounter(umpk);

        digit0 = UMPK80_GetDisplayDigit(umpk, 0);
        digit1 = UMPK80_GetDisplayDigit(umpk, 1);
        digit2 = UMPK80_GetDisplayDigit(umpk, 2);
        digit3 = UMPK80_GetDisplayDigit(umpk, 3);

        digit4 = UMPK80_GetDisplayDigit(umpk, 4);
        digit5 = UMPK80_GetDisplayDigit(umpk, 5);
      }

    }
  });

  tauri::Builder::default()
    .invoke_handler(tauri::generate_handler![progress_tracker])
    .run(tauri::generate_context!())
    .expect("error while running tauri application");
}
