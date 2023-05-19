cmake . -B ./build
make ./build -j4
echo ".uf2 file generated under ./build/src/main.uf2 (from within build directory)"
echo "copy it to your pico board with:"
echo "  MacOS: cp ./build/src/main.uf2 /Volumes/RPI-RP2"
echo "  LINUX: TODO"
echo "  WINDOWS: TODO"