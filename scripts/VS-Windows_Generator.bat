@echo off
cd ..

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="./vendor/vcpkg/scripts/buildsystems/vcpkg.cmake" -G "Visual Studio 17 2022"

pause