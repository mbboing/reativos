cd
sudo apt-get update
sudo apt-get install mercurial
sudo apt-get install libxext-dev
hg clone https://hg.libsdl.org/SDL SDL
cd SDL
mkdir build
cd build
../configure
make
sudo make install
# Para compilar algum programa, use: gcc -o myprogram myprogram.c `sdl2-config --cflags --libs`
