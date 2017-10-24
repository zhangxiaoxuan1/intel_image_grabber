# Requirements

OpenCV and librealsense should be installed in your system.

# Instructions for building & running

Navigate to ./build and type 'cmake ..' followed by 'make'

This will put a binary called 'snapshot' into the ./build/bin directory.

Before running the program, change the config.toml file to your specifications.

When running the program, pass the config file path as an argument.

To load the default config file, run ./snapshot ../../config/config.toml

# Configurations

## resolution and frame rate

R200/SR300 accepts the following configurations:

* @60FPS, depth at 320x240, color can be 640x480
* @60FPS, depth at 480x360,  color can be 320x240 or 640x480
* @30FPS, depth at 320x240, color can be 640x480 or 1920x1080
* @30FPS, depth at 480x360, color can be 320x240, 640x480, or 1920x1080

Note that this documentation is old and other resolutions are also supported, for instance @60FPS 640*480 for both depth and color. Currently I haven't found an up-to-date documentation.

## auto_exposure_mean_intensity

Value from 0 to 4095. Larger value leads to brighter IR image.

## dp_preset

This provides access to several recommend sets of depth control parameters. Default is 0. This is only working for R200.

* __0-DEFAULT__   Default settings on chip. Similar to the medium setting and best for outdoors.
* __1-OFF__       Disable almost all hardware-based outlier removal.
* __2-LOW__       Provide a depthmap with a lower number of outliers removed, which has minimal false negatives.
* __3-MEDIUM__    Provide a depthmap with a medium number of outliers removed, which has balanced approach.
* __4-OPTIMIZED__ Provide a depthmap with a medium/high number of outliers removed. Derived from an optimization function.
* __5-HIGH__      Provide a depthmap with a higher number of outliers removed, which has minimal false positives.

Parameters influenced, in order:

* RS_OPTION_R200_DEPTH_CONTROL_ESTIMATE_MEDIAN_DECREMENT,
* RS_OPTION_R200_DEPTH_CONTROL_ESTIMATE_MEDIAN_INCREMENT,
* RS_OPTION_R200_DEPTH_CONTROL_MEDIAN_THRESHOLD,
* RS_OPTION_R200_DEPTH_CONTROL_SCORE_MINIMUM_THRESHOLD,
* RS_OPTION_R200_DEPTH_CONTROL_SCORE_MAXIMUM_THRESHOLD,
* RS_OPTION_R200_DEPTH_CONTROL_TEXTURE_COUNT_THRESHOLD,
* RS_OPTION_R200_DEPTH_CONTROL_TEXTURE_DIFFERENCE_THRESHOLD,
* RS_OPTION_R200_DEPTH_CONTROL_SECOND_PEAK_THRESHOLD,
* RS_OPTION_R200_DEPTH_CONTROL_NEIGHBOR_THRESHOLD,
* RS_OPTION_R200_DEPTH_CONTROL_LR_THRESHOLD

# Notes

If not using cmake, compile the cpp file without cmake: g++ -std=c++11 snapshot_intel.cpp -lrealsense -lopencv_core -lopencv_highgui

If you want to run the klg output with Kintinuous or ElasticFusion, run

`
./ElasticFusion -l (path to snapshot.klg) -d 12 -c 3 -f
`
