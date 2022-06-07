rm ./out
cd ./build
rm -rf *
cmake ..
make
cp ./compile_commands.json ../
cd ..
#./out
./Bus-operation > ./output.io < ./input.io

