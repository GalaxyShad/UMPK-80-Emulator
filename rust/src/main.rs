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

fn main() {

    let mut file = File::open("../data/scaned-os-fixed.bin")
        .expect("Failed to open file");

    let mut contents = Vec::new();
    file.read_to_end(&mut contents)
        .expect("Failed to read file");

    //println!("{:?}", contents);

    unsafe {
        let umpk = UMPK80_Create();
        
        UMPK80_LoadOS(umpk, contents.as_mut_ptr());

        loop {
            UMPK80_Tick(umpk);
            
            let pg = UMPK80_Cpu_GetProgramCounter(umpk);

            let digit = UMPK80_GetDisplayDigit(umpk, 0);

            println!("PG = {pg:04X} {digit:02X}");
        }

        UMPK80_Free(umpk);
    }

    println!("Hello, world!");
}
