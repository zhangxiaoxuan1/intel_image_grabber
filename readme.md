# Requirements

OpenCV and librealsense should be installed in your system.

# Instructions for building & running

Navigate to ./build and type 'cmake ..' followed by 'make'

This will put a binary called 'snapshot' into the ./build/bin directory.

Before running the program, change the config.toml file to your specifications.

When running the program, pass the config file path as an argument.

To load the default config file, run ./snapshot ../../config/config.toml

# Notes

Compiling the cpp file without cmake: g++ -std=c++11 snapshot_intel.cpp -lrealsense -lopencv_core -lopencv_highgui
