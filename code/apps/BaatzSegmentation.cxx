/*=========================================================================

  Program: Example of application of the Generic Region Merging Library
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// Boost headers (mandatory)
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

// Your algorithm header file
#include "baatz-algorithm.h"

namespace po = boost::program_options;
bool init_args(int argc, char ** argv, po::options_description& desc, po::variables_map& vm);

int main(int argc, char **argv)
{
	po::options_description desc("Configuration of the Baatz & Schâpe region merging segmentation");
	po::variables_map vm;

	if(init_args(argc, argv, desc, vm))
	{
		typedef otb::VectorImage<float, 2> ImageType;
		typedef BaatzAlgorithmRM<ImageType> SegmenterType;

		BaatzParams params = {vm["cw"].as<float>(), vm["sw"].as<float>(), vm["sp"].as<float>()};
		SegmenterType seg_;
		seg_.SetInput(vm["input"].as<std::string>());
		seg_.SetOutputRGB(vm["output_rgb"].as<std::string>());
		seg_.SetParameters(params);

		seg_.InitFromImage();
		seg_.Segmentation();
		seg_.WriteRGBOutputImage();

		return 0;
	}
	else
		return 1;
}

bool init_args(int argc, char ** argv, po::options_description& desc, po::variables_map& vm)
{
	desc.add_options()
		("help", "print mandatory arguments")
		("input", po::value<std::string>(), "set input image file (mandatory)")
		("output_rgb", po::value<std::string>(), "set output rgb image file (mandatory)")
		("cw", po::value<float>(), "set the spectral weight (mandatory)")
		("sw", po::value<float>(), "set the shape weight (mandatory)")
		("sp", po::value<float>(), "set the scale parameter (mandatory)")
		("iter", po::value<unsigned int>(), "set the number of iterations using LMBF (optional)")
		("bf", po::value<int>(), "activate the Best Fitting Heuristic (optional)");

	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return false;
	}

	if (!vm.count("input")) {
		std::cout << "The input image file was not set (--input).\n";
		std::cout << desc << "\n";
		return false;
	}

	if (!vm.count("output_rgb")) {
		std::cout << "The output rgb file was not set (--output_rgb).\n";
		std::cout << desc << "\n";
		return false;
	}

	if(!vm.count("cw")){
		std::cout << "The spectral weight was not set (--cw).\n";
		std::cout << desc << "\n";
		return false;
	}

	if(!vm.count("sw")){
		std::cout << "The shape weight was not set (--sw).\n";
		std::cout << desc << "\n";
		return false;
	}

	if(!vm.count("sp")){
		std::cout << "The scale parameter was not set (--sp).\n";
		std::cout << desc << "\n";
		return false;
	}

	return true;
}
