#To rebuild from scratch
cd ~/huevadas/projects/geometry && rm -rf build && mkdir -p build && cd build && cmake .. && make ; cd ..

#To build
make

#To test
make test