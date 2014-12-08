/*=========================================================================

  Program: Example of application of the Generic Region Merging Library
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <iostream>

// Your algorithm header file
#include "baatz-algorithm.h"

int main(int argc, char **argv)
{
	if(argc < 7 || argc > 9)
	{
		std::cout << "###############      Manual     #############################\n\n\n"
				<< "Usage " << argv[0] << "\n\n\n"
				<< " -- [input image (tif, jpg, png) <mandatory>]\n"
				<< " -- [output rgb image (tif, jpg, png) <mandatory>]\n"
				<< " -- [output label image (tif) <mandatory>]\n"
				<< " -- [spectral weight (value >= 0 and <= 1) <mandatory>]\n"
				<< " -- [shape weight (value >= 0 and <= 1) <mandatory>]\n"
				<< " -- [scale parameter (positive value) <mandatory>]\n"
				<< " -- [number of iterations using the Local Mutual Best Fitting heuristic (70 by default) <optional>]\n"
				<< " -- [activation of the Best Fitting heuristic (value = 0 (desactivated) or 1 (activated) \
					- activated by default) <optional>]\n\n\n"
				<< "################################################################\n";
		return 1;
	}

	const std::string inputFileName = argv[1];
	const std::string outputFileName = argv[2];
	const std::string outputLabelFileName = argv[3];
	const float cw = atof(argv[4]);
	const float sw = atof(argv[5]);
	const float sp = atof(argv[6]);

	typedef otb::VectorImage<float, 2> ImageType;
	typedef BaatzAlgorithmRM<ImageType> SegmenterType;

	BaatzParams params = {cw, sw, sp};
	SegmenterType seg_;
	seg_.SetInput(inputFileName);
	seg_.SetOutputRGB(outputFileName);
	seg_.SetOutputLabel(outputLabelFileName);
	seg_.SetParameters(params);
	if(argc >= 8)
		seg_.SetNumberOfIterations(static_cast<unsigned int>(atoi(argv[7])));
	if(argc == 9)
		seg_.SetBestFitting(atoi(argv[8]));

	seg_.Run();

	return 0;
}
