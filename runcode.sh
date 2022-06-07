rm ./Bus-operation
rm -rf ./build
mkdir build
cd ./build
cmake ..
make
cp ./compile_commands.json ../
cd ..
./Bus-operation > ./output.io < ./input.io

