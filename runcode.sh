rm ./out
cd ./build
rm -rf *
cmake ..
make
cp ./compile_commands.json ../
cd ..
#./out
./out > ./output.io < ./input.io

