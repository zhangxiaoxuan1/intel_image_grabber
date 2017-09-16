//include the librealsense C++ header file
#include <librealsense/rs.hpp>

// include opencv
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include "toml.h"

int frames_written = 0;
std::string name_suffix = ".png";
std::string config_file;

int capture_num;
int frame_rate;
int dc_preset;

bool depth_enable;
bool depth_plot;
bool depth_write;
std::string depth_path;
std::string depth_prefix;
std::string depth_snap_prefix;
bool depth_directory_created = false;
int depth_width;
int depth_height;

bool rgb_enable;
bool rgb_plot;
bool rgb_write;
std::string rgb_path;
std::string rgb_prefix;
std::string rgb_snap_prefix;
bool rgb_directory_created = false;
int rgb_width;
int rgb_height;

bool ir_enable;
bool ir_plot;
bool ir_write;
std::string ir_path;
std::string ir_prefix;
std::string ir_snap_prefix;
bool ir_directory_created = false;
int ir_width;
int ir_height;

bool load_config() {
	std::ifstream input(config_file);

	toml::ParseResult pr = toml::parse(input);

	if (!pr.valid()) {
		std::cerr << "Could not load config file: " << pr.errorReason
				<< std::endl;
		return false;
	}

	// pr.value is the parsed value.
	const toml::Value& v = pr.value;

	dc_preset = v.get<int>("settings.dc_preset");
	capture_num = v.get<int>("settings.frame_number");
	frame_rate = v.get<int>("settings.frame_rate");

	depth_enable = v.get<bool>("depth.enable");
	depth_plot = v.get<bool>("depth.display");
	depth_write = v.get<bool>("depth.export");
	depth_path = v.get<std::string>("depth.directory");
	depth_prefix = v.get<std::string>("depth.export_prefix");
	depth_snap_prefix = v.get<std::string>("depth.snapshot_prefix");
	depth_width = v.get<int>("depth.width");
	depth_height = v.get<int>("depth.height");

	rgb_enable = v.get<bool>("color.enable");
	rgb_plot = v.get<bool>("color.display");
	rgb_write = v.get<bool>("color.export");
	rgb_path = v.get<std::string>("color.directory");
	rgb_prefix = v.get<std::string>("color.export_prefix");
	rgb_snap_prefix = v.get<std::string>("color.snapshot_prefix");
	rgb_width = v.get<int>("color.width");
	rgb_height = v.get<int>("color.height");

	ir_enable = v.get<bool>("ir.enable");
	ir_plot = v.get<bool>("ir.display");
	ir_write = v.get<bool>("ir.export");
	ir_path = v.get<std::string>("ir.directory");
	ir_prefix = v.get<std::string>("ir.export_prefix");
	ir_snap_prefix = v.get<std::string>("ir.snapshot_prefix");
	ir_width = v.get<int>("ir.width");
	ir_height = v.get<int>("ir.height");

	return true;
}

void make_depth_histogram(const cv::Mat &depth, cv::Mat &colored_depth) {
  colored_depth = cv::Mat(depth.size(), CV_8UC3);

  static uint32_t histogram[0x10000];
  memset(histogram, 0, sizeof(histogram));

  for(int i = 0; i < depth_height; ++i) {
    for (int j = 0; j < depth_width; ++j) {
      ++histogram[depth.at<ushort>(i,j)];
    }
  }

  for(int i = 2; i < 0x10000; ++i) histogram[i] += histogram[i-1]; // Build a cumulative histogram for the indices in [1,0xFFFF]

  for(int i = 0; i < depth_height; ++i) {
    for (int j = 0; j < depth_width; ++j) {
      if (uint16_t d = depth.at<ushort>(i,j)) {
        int f = histogram[d] * 255 / histogram[0xFFFF]; // 0-255 based on histogram location
        colored_depth.at<cv::Vec3b>(i,j) = cv::Vec3b(f, 0, 255-f);
      } else {
    	  colored_depth.at<cv::Vec3b>(i,j) = cv::Vec3b(0, 5, 20);
      }
    }
  }
}

int main(int argc, char* argv[])
try {

	if (argc != 2) { // argc should be 2 for correct execution
		// We print argv[0] assuming it is the program name
		std::cerr << "usage: " << argv[0] << " <configuration-file>"
				<< std::endl;
		return -1;
	} else {
		config_file = std::string(argv[1]);
	}
	std::cerr << "Loading configuration file...";
	if (!load_config()){
		printf("Failed to load configuration.\n");
		return EXIT_FAILURE;
	}
	std::cerr << "done!\n" << std::endl;

	if (!depth_enable && !rgb_enable && !ir_enable) {
		printf("Error: No streams enabled.\n");
		return EXIT_FAILURE;
	}

	// Create a context object. This object owns the handles to all connected realsense devices.
	rs::context ctx;
	printf("You have %d connected RealSense devices.\n",
			ctx.get_device_count());
	if (ctx.get_device_count() == 0) {
		printf("You have 0 device connected. Please check connection.\n");
		return EXIT_FAILURE;
	}

	// Access the device
	rs::device * dev = ctx.get_device(0);
	printf("Using device 0, an %s\n", dev->get_name());
	printf("    Serial number: %s\n", dev->get_serial());
	printf("    Firmware version: %s\n", dev->get_firmware_version());

	// Configure streams
	if (frame_rate != 30 && frame_rate != 60){
		printf("Error: Frame rate must be 30 or 60.\n");
		return EXIT_FAILURE;
	}
	if (depth_enable) {
		dev->enable_stream(rs::stream::depth, depth_width, depth_height, rs::format::z16, frame_rate);
	}
	if (rgb_enable) {
		dev->enable_stream(rs::stream::color, rgb_width, rgb_height, rs::format::rgb8, frame_rate);
	}
	if (ir_enable) {
		dev->enable_stream(rs::stream::infrared, ir_width, ir_height, rs::format::y8, frame_rate);
	}
	if (!depth_enable && !rgb_enable && !ir_enable) {
		printf("No stream enabled, program exits automatically.\n");
		return EXIT_SUCCESS;
	}
	// Configure camera
	if (ctx.get_device_count() == 0) {
		printf("You have 0 device connected. Please check connection.\n");
		return EXIT_FAILURE;
	}

	if (dc_preset < 0 || dc_preset > 5){
		printf("Error: re_preset must be between 0 and 5. Check readme for configuration details.\n");
		return EXIT_FAILURE;
	}
	rs_apply_depth_control_preset((rs_device *)dev, dc_preset);
	dev->start();

	// Camera warmup - Dropped several first frames to let auto-exposure stabilize
	for (int i = 0; i < 30; i++) {
		dev->wait_for_frames();
	}

	// Create directories to write to
	if (depth_write) {
		if (system(("mkdir -p " + depth_path).c_str()) !=0) {
	        throw std::runtime_error("Could not create directory");
		}
		depth_directory_created = true;
	}
	if (rgb_write) {
		if (system(("mkdir -p " + rgb_path).c_str()) !=0) {
	        throw std::runtime_error("Could not create directory");
		}
		rgb_directory_created = true;
	}
	if (ir_write) {
		if (system(("mkdir -p " + ir_path).c_str()) !=0) {
	        throw std::runtime_error("Could not create directory");
		}
		ir_directory_created = true;
	}

	cv::Mat color;    //(height, width, CV_8UC3);
	cv::Mat depth;    //(height, width, CV_16UC1);
	cv::Mat ir;    //(height, width, CV_16UC1);
	for (int i = 0; i < capture_num; i++) {

		// Wait for frame to load
		if (dev->is_streaming())
			dev->wait_for_frames();

		if (depth_enable) {
			depth = cv::Mat(cv::Size(depth_width, depth_height), CV_16UC1,
					(void*) dev->get_frame_data(rs::stream::depth),
					cv::Mat::AUTO_STEP);
			if (depth_write) {
				std::stringstream ss;
				ss << depth_path << depth_prefix << std::setw(5)
						<< std::setfill('0') << i << name_suffix;
				cv::imwrite(ss.str(), depth);
			}
			if (depth_plot) {
				// Convert 16bit to 8 bit color
				cv::Mat depth_show;
				make_depth_histogram(depth, depth_show);
				cv::imshow("Depth", depth_show);
			}
		}
		if (rgb_enable) {
			color = cv::Mat(cv::Size(rgb_width, rgb_height), CV_8UC3,
					(void*) dev->get_frame_data(rs::stream::color),
					cv::Mat::AUTO_STEP);
			cv::cvtColor(color, color, cv::COLOR_BGR2RGB );
			if (rgb_write) {
				std::stringstream ss;
				ss << rgb_path << rgb_prefix << std::setw(5)
						<< std::setfill('0') << i << name_suffix;
				cv::imwrite(ss.str(), color);
			}
			if (rgb_plot) {
				cv::imshow("RGB", color);
			}
		}
		if (ir_enable) {
			ir = cv::Mat(cv::Size(ir_width, ir_height), CV_8UC1,
					(void*) dev->get_frame_data(rs::stream::infrared),
					cv::Mat::AUTO_STEP);
			if (ir_write) {
				std::stringstream ss;
				ss << ir_path << ir_prefix << std::setw(5) << std::setfill('0')
						<< i << name_suffix;
				cv::imwrite(ss.str(), ir);
			}
			if (ir_plot) {
				cv::imshow("IR", ir);
			}
		}
		int key = cv::waitKey(1);
		switch (key) {
		case 27:
			printf("Escape key pressed. Exiting program...\n");
			return EXIT_SUCCESS;
		case 32:
			if (depth_enable) {
				if (!depth_directory_created) {
					if (system(("mkdir -p " + depth_path).c_str()) !=0) {
				        throw std::runtime_error("Could not create directory");
					}
					depth_directory_created = true;
				}
				std::stringstream ss;
				ss << depth_path << depth_snap_prefix << std::setw(5)
						<< std::setfill('0') << frames_written << name_suffix;
				cv::imwrite(ss.str(), depth);
			}
			if (rgb_enable) {
				if (!rgb_directory_created) {
					if (system(("mkdir -p " + rgb_path).c_str()) !=0) {
				        throw std::runtime_error("Could not create directory");
					}
					rgb_directory_created = true;
				}
				std::stringstream ss;
				ss << rgb_path << rgb_snap_prefix << std::setw(5)
						<< std::setfill('0') << frames_written << name_suffix;
				cv::imwrite(ss.str(), color);
			}
			if (ir_enable) {
				if (!ir_directory_created) {
					if (system(("mkdir -p " + ir_path).c_str()) !=0) {
				        throw std::runtime_error("Could not create directory");
					}
					ir_directory_created = true;
				}
				std::stringstream ss;
				ss << ir_path << ir_snap_prefix << std::setw(5)
						<< std::setfill('0') << frames_written << name_suffix;
				cv::imwrite(ss.str(), ir);
			}
			frames_written++;
			printf("Snapshot is taken.\n");
		}
	}
	return EXIT_SUCCESS;

}
catch (const rs::error & e) {
	// Method calls against librealsense objects may throw exceptions of type rs::error
	printf("rs::error was thrown when calling %s(%s):\n",
			e.get_failed_function().c_str(), e.get_failed_args().c_str());
	printf("    %s\n", e.what());
	return EXIT_FAILURE;
}
