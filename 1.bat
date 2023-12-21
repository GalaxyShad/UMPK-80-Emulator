cmake -S . -B build -G "Ninja"
cmake --build build --config Debug

cd build/bin
umpk-80-emu-ui.exe
cd ..