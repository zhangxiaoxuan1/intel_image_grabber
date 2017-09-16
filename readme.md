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

# dp_preset

This provides access to several recommend sets of depth control parameters. Default is 0.

0 {5, 5, 192,  1,  512, 6, 24, 27,  7,   24}, /* (DEFAULT)   Default settings on chip. Similar to the medium setting and best for outdoors. */

1 {5, 5,   0,  0, 1023, 0,  0,  0,  0, 2047}, /* (OFF)       Disable almost all hardware-based outlier removal */

2 {5, 5, 115,  1,  512, 6, 18, 25,  3,   24}, /* (LOW)       Provide a depthmap with a lower number of outliers removed, which has minimal false negatives. */

3 {5, 5, 185,  5,  505, 6, 35, 45, 45,   14}, /* (MEDIUM)    Provide a depthmap with a medium number of outliers removed, which has balanced approach. */

4 {5, 5, 175, 24,  430, 6, 48, 47, 24,   12}, /* (OPTIMIZED) Provide a depthmap with a medium/high number of outliers removed. Derived from an optimization function. */

5 {5, 5, 235, 27,  420, 8, 80, 70, 90,   12}, /* (HIGH)      Provide a depthmap with a higher number of outliers removed, which has minimal false positives. */

Parameters influenced:
`
{
        RS_OPTION_R200_DEPTH_CONTROL_ESTIMATE_MEDIAN_DECREMENT,
        RS_OPTION_R200_DEPTH_CONTROL_ESTIMATE_MEDIAN_INCREMENT,
        RS_OPTION_R200_DEPTH_CONTROL_MEDIAN_THRESHOLD,
        RS_OPTION_R200_DEPTH_CONTROL_SCORE_MINIMUM_THRESHOLD,
        RS_OPTION_R200_DEPTH_CONTROL_SCORE_MAXIMUM_THRESHOLD,
        RS_OPTION_R200_DEPTH_CONTROL_TEXTURE_COUNT_THRESHOLD, 
        RS_OPTION_R200_DEPTH_CONTROL_TEXTURE_DIFFERENCE_THRESHOLD,
        RS_OPTION_R200_DEPTH_CONTROL_SECOND_PEAK_THRESHOLD,
        RS_OPTION_R200_DEPTH_CONTROL_NEIGHBOR_THRESHOLD,
        RS_OPTION_R200_DEPTH_CONTROL_LR_THRESHOLD
    };
`

