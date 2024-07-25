# UMPK-80 Emulator (CORE)

![image](https://github.com/GalaxyShad/UMPK-80-Emulator/assets/52833080/1b383c7a-b495-4e51-862b-9ec07bb2d46c)

The UMPK-80 Emulator (CORE) is an open-source project that aims to provide a simple and modern INTEL 8080 / KR580VM80A-based microcomputer emulator.

## What is UMPK-80?

The [UMPK-80/VM](https://retro-computer.ru/home.aspx#/item/UMPK_80) is a laboratory workbench developed in the USSR in 1987 at the Moscow Institute of Electronic Technology (MIET). It is designed for studying programming and the operation of the MP BIS KR580VM80A (the Soviet equivalent of the INTEL 8080), building microprocessor systems, and using as a controlling device when researching the operation of interface modules.

## Note

This project serves more as an external API and GUI for its debugging. For a more user-friendly interface, please refer to the [UMPK80-IDE](https://github.com/GalaxyShad/UMPK80-IDE) project.

## Related Projects

- [Some-i8080-ASM-Translator](https://github.com/GalaxyShad/Some-i8080-ASM-Translator) - A modern and fast INTEL 8080 / KR580VM80A assembler translator created from scratch on .NET 8.0 and C#.
- [UMPK80-IDE](https://github.com/GalaxyShad/UMPK80-IDE) - A more user-friendly implementation of the emulator with a convenient interface and features, created on Tauri, React, Rust, and TS.
- [ASM-I8080A-Programs-Collection](https://github.com/GalaxyShad/ASM-I8080A-Programs-Collection) - Examples of programs written for the UMPK-80.
- [UMPK80-Monitor-System](https://github.com/GalaxyShad/UMPK80-Monitor-System) - The source code of the DM80 firmware with comments.

# Features:

* **Full KR580VM80A (INTEL 8080) processor emulation:** Includes support for undocumented instructions such as 08h, 10h, 18h, 20h, 28h, 30h, 38h, NOP 0CBh, JMP 0D9h, RET 0DDh, 0EDh, 0FDh, and CALL.
* **C++11:** The emulator is built using the latest C++ standards for improved performance and reliability.
* **Standalone core:** The core emulation engine is independent of the standard C++ library, making it easy to integrate with other projects.
* **External API:** The emulator provides an external API (core/cumpk80) for integration with other products and tools.
* **Original DM80 firmware emulation:** The emulator faithfully recreates the original DM80 firmware taken from a real UMPK-80 workbench.
* **Peripheral device emulation:** The emulator emulates a range of peripheral devices, including the keyboard, display, and printer.
* **Sound emulation:** The emulator can simulate the sounds of the UMPK-80, subject to certain limitations (see the "Limitations" section).
* **Real-time RAM editor:** The emulator includes a powerful RAM editor that allows you to modify the contents of memory in real-time.
* **Full control over the processor and firmware:** The emulator provides complete control over the processor and firmware, allowing you to pause, step through, and debug your code.
* **Compact mode:** The emulator can be launched in compact mode by providing a bin file as an argument, making it easy to use in a variety of environments.
* **Dock layout:** The emulator features a customizable dock layout, allowing you to arrange the tools and windows in a way that suits your workflow.
* **SFML:** The emulator is built using the SFML library, providing a robust and efficient foundation for the emulation engine.
* **ImGui:** The emulator uses the ImGui library for its user interface, providing a fast and responsive experience.
* **Disassembler:** The emulator includes a built-in disassembler, making it easy to analyze and understand the behavior of your code.
* **Integrated table with opcode commands:** The emulator includes an integrated table of opcode commands, providing a quick and easy reference for the various instructions supported by the KR580VM80A (INTEL 8080) processor.

## Limitations

- The emulated processor runs at a faster speed than the original processor.
- Since the original workbench generates the sound wave itself by sending data to the dynamic port, which cannot be achieved in emulation, the emulator simply catches the call of the sound output subroutine with the desired frequency (address 0447h).
- Currently, only the UMPK-80/VM keyboard module is emulated. The emulation of the connectable modules UMPK-80/MI 1 - UMPK-80/MI 6, UMPK-80/MR 1 - UMPK-80/MR 11, UMPK-80/MO, UMPK-80/MT is not available.

## Good Information Sources

- [INTEL 8080 Assembly Programming Manual](https://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf) - The official manual for programming on the Intel 8080.
- [ALTAIR 8800 INSTRUCTION SET](https://ubuntourist.codeberg.page/Altair-8800/part-4.html) - A well-structured documentation of the Intel 8080 instructions.
- [INTEL 8080 instruction set table](https://pastraiser.com/cpu/i8080/i8080_opcodes.html) - A well-formatted table of the Intel 8080 opcodes.
- [Workbench Photos](https://retro-computer.ru/home.aspx#/item/UMPK_80) - A gallery of workbench photos from the museum.
- Book: "Microcomputers. Book 7. Training Stands. Practical Manual." Edited by L.N. Presnukhin. Authors: Yu.I. Volkov, V.L. Gorbunov, D.I. Panfilov, S.G. Shar

## Some User Subroutines
| Subroutine Name | Starting Address | Input Parameters | Output Parameters |
| --- | --- | --- | --- |
| Melody 1 | 05BAh | None | None |
| Melody 2 | 05B0h | None | None |
| Organ | 04FCh | Codes of the numeric field keys | None |
| Stopwatch | 0481h | Code of the key "0" - reset, code of the key "1" - start/stop | The current time is displayed on the screen every 1 second |
| Fixed delay of 1 ms | 0429h | None | None |
| Adjustable delay | 0430h | The number of milliseconds in the BC register pair | None |
| Multiplication of one-byte numbers | 04E1h | Multiplier in the E register, multiplicand in the D register | The product in the BC register pair |
| Message rewrite in the storage area | By the RST3 interrupt command programmatically | The address of the first byte of the message is placed in the DE register pair | Six bytes of the message are placed in the cells at addresses 0BF0h-0BF5h |
| Decoding a message for display on the screen | 019h | Message codes in the cells 0BF0h-0BF5h | Seven-segment codes of six bytes of the message in the cells 0BFAh-0BFFh |
| Key press detection | 0185h | Pressing numeric or functional keys (excluding "St" and "R" keys) | Flag Z = 0 - when the key is pressed, and Z = 1 - when it is not pressed |
| Keyboard scanning | 014Bh | Key codes according to the conversion table (the code assigned to the key) | The key code in the A register |
| One-time screen scan | 01CBh | Seven-segment codes in the cells 0BFAh-0BFFh | One-time display on the screen |

## Download

You can download the latest executable version of this project in the [Releases section](https://github.com/GalaxyShad/UMPK-80-Emulator/releases).

## Build

Clone the repository:

```
git clone https://github.com/GalaxyShad/UMPK-80-Emulator.git && cd UMPK-80-Emulator
```

Build using CMake:

```
cmake -S . -B build
cmake --build build --config Release
```
## Media
![33](https://github.com/user-attachments/assets/48c12932-f516-477a-b5ab-4b9d892b20ac)


## Real-life Photo

![image](https://github.com/GalaxyShad/UMPK-80-Emulator/assets/52833080/d6aa042b-546e-4c7b-b36f-1ecd90172afc)
