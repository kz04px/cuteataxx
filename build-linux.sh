mkdir ./libs/libataxx/build
cd ./libs/libataxx/build
cmake ..
make static

cd ../../../
mkdir ./build
cd build
cmake ..
make
