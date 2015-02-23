#include <iostream>
#include <otbImage.h>
#include <otbImageFileReader.h>
#include <otbVectorImage.h>
#include "lsrmBaatzSegmenter.h"

int main(int argc, char *argv[])
{
	if(argc != 7)
	{
		std::cerr << "Usage: ./" << argv[0] << "\n"
				  << "\t[input image path] : (.jpg, .png, .tif)\n"
				  << "\t[output clustered image] : (.jpg, .png, .tif)\n"
				  << "\t[output label image] : (.tif)\n"
				  << "\t[spectral weight] : range from 0 to 1\n"
				  << "\t[shape weight] : range from 0 to 1\n"
				  << "\t[scale threshold] : unlimited positive value"
				  << std::endl;
		return 1;
	}

	lsrm::BaatzParam params;
	const char * input_image = argv[1];
	const char * clustered_image = argv[2];
	const char * label_image = argv[3];
	params.m_SpectralWeight = atof(argv[4]);
	params.m_ShapeWeight = atof(argv[5]);
	float sqrt_scale = atof(argv[6]);
	const float scale = sqrt_scale * sqrt_scale;

	typedef float PixelType;
	typedef otb::VectorImage<PixelType, 2> ImageType;
	typedef lsrm::BaatzSegmenter<ImageType> SegmenterType;

	SegmenterType segmenter;
	segmenter.SetParam(params);
	segmenter.SetThreshold(scale);
	segmenter.SetInputFileName(input_image);
	segmenter.SetClusteredImageFileName(clustered_image);
	segmenter.SetLabelImageFileName(label_image);
	
	segmenter.RunSegmentation();
	
    return 0;
}







