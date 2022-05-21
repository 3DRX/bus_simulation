rm ./out
cd ./build
rm -rf *
cmake ..
make
cd ..
#./out
./out > ./output.io < ./input.io

