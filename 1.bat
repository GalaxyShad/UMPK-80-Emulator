g++ src/*.cpp lib/*.cpp -I lib -L lib/lib -lsfml-window -lsfml-system -lsfml-audio -lsfml-graphics -lfreeglut -lopengl32 -o build/app.exe

cd build
app.exe
cd ..