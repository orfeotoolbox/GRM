#include <iostream>
#include <otbImage.h>
#include <otbVectorImage.h>
#include <otbImageFileReader.h>
#include <otbImageFileWriter.h>
#include "lsrmFullLambdaScheduleSegmenter.h"

int main(int argc, char *argv[])
{
	if(argc != 5)
	{
		std::cerr << "Usage: ./" << argv[0] << "\n"
				  << "\t[input image path] : (.jpg, .png, .tif)\n"
				  << "\t[output clustered image] : (.jpg, .png, .tif)\n"
				  << "\t[output label image] : (.tif)\n"
				  << "\t[lambda threshold] : unlimited positive value"
				  << std::endl;
		return 1;
	}

	const char * inFileName = argv[1];
	const char * clusterFileName = argv[2];
	const char * labelFileName = argv[3];
	const float lambda = atof(argv[4]);

	typedef float PixelType;
	typedef unsigned long int LabelPixelType;
	typedef unsigned char ClusterPixelType;
	typedef otb::VectorImage<PixelType, 2> InputImageType;
	typedef otb::Image<LabelPixelType, 2> LabelImageType;
	typedef otb::VectorImage<ClusterPixelType, 2> ClusterImageType;
	typedef otb::ImageFileReader<InputImageType> InputImageReaderType;
	typedef otb::ImageFileWriter<LabelImageType> LabelImageWriterType;
	typedef otb::ImageFileWriter<ClusterImageType> ClusterImageWriterType;
	typedef lsrm::FullLambdaScheduleSegmenter<InputImageType> SegmenterType;

	auto imgReader = InputImageReaderType::New();
	imgReader->SetFileName(inFileName);
	imgReader->Update();

	
	SegmenterType segmenter;
	segmenter.SetThreshold(lambda);
	segmenter.SetInput(imgReader->GetOutput());
	segmenter.Update();

	auto labelWriter = LabelImageWriterType::New();
	labelWriter->SetFileName(labelFileName);
	labelWriter->SetInput(segmenter.GetLabeledClusteredOutput());
	labelWriter->Update();

	auto clusterWriter = ClusterImageWriterType::New();
	clusterWriter->SetFileName(clusterFileName);
	clusterWriter->SetInput(segmenter.GetClusteredImageOutput());
	clusterWriter->Update();
	
    return 0;
}
