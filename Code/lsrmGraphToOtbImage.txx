#ifndef __LSRM_GRAPH_TO_OTBIMAGE_TXX
#define __LSRM_GRAPH_TO_OTBIMAGE_TXX
#include "lsrmGraphToOtbImage.h"

namespace lsrm
{
	template<class TGraph>
	void GraphToOtbImage<TGraph>::WriteLabelImage(const GraphType& graph,
												  const unsigned int width,
												  const unsigned int height,
												  const std::string& outputFileName)
	{
		typedef long unsigned int LabelPixelType;

		typedef otb::Image<LabelPixelType, 2> LabelImageType;

		LabelImageType::IndexType index;
		LabelImageType::SizeType size;
		LabelImageType::RegionType region;

		index[0] = 0; index[1] = 0;
		size[0] = width; size[1] = height;
		region.SetIndex(index);
		region.SetSize(size);

		LabelImageType::Pointer label_img = LabelImageType::New();
		label_img->SetRegions(region);
		label_img->Allocate();
		
		// Start at 1 (value 0 can be used for invalid pixels)
		long unsigned int label = 1;
		for(auto& region : graph.m_Nodes)
		{

			for (auto& pix: region->m_Pixels)
			{
				index[0] = pix % width;
				index[1] = pix / width;
				label_img->SetPixel(index, label);
			}
			++label;
		}

		typedef otb::ImageFileWriter<LabelImageType> LabelWriterType;
		LabelWriterType::Pointer label_writer = LabelWriterType::New();
		label_writer->SetInput(label_img);
		label_writer->SetFileName(outputFileName);
		label_writer->Update();
	}

	template<class TGraph>
	void GraphToOtbImage<TGraph>::WriteOutputRGBImage(const GraphType& graph,
													  const unsigned int width,
													  const unsigned int height,
													  const std::string& outputFileName)
	{
		typedef unsigned char RGBPixelType;

		typedef otb::VectorImage<RGBPixelType, 2> RGBImageType;
		typename RGBImageType::PixelType pixelValue;

		RGBImageType::IndexType index;
		RGBImageType::SizeType size;
		RGBImageType::RegionType region;

		index[0] = 0; index[1] = 0;
		size[0] = width; size[1] = height;
		region.SetIndex(index);
		region.SetSize(size);

		RGBImageType::Pointer rgb_img = RGBImageType::New();
		rgb_img->SetRegions(region);
		rgb_img->SetNumberOfComponentsPerPixel(3);
		rgb_img->Allocate();
		pixelValue.Reserve(3);
		pixelValue[0] = 0;
		pixelValue[1] = 0;
		pixelValue[2] = 0;

		for(unsigned int y = 0; y < height; ++y)
		{
			for(unsigned int x = 0; x < width; ++x)
			{
				index[0] = x;
				index[1] = y;
				rgb_img->SetPixel(index, pixelValue);
			}
		}

		srand(time(NULL));
		unsigned char c1, c2, c3;
		for(auto& region : graph.m_Nodes)
		{
			c1 = rand() % 256;
			c2 = rand() % 256;
			c3 = rand() % 256;
			for (auto& pix : region->m_Pixels)
			{
				index[0] = pix % width;
				index[1] = pix / width;
				pixelValue[0] = c1;
				pixelValue[1] = c2;
				pixelValue[2] = c3;
				rgb_img->SetPixel(index, pixelValue);
			}
		}

		typedef otb::ImageFileWriter<RGBImageType> RGBWriterType;
		RGBWriterType::Pointer rgb_writer = RGBWriterType::New();
		rgb_writer->SetInput(rgb_img);
		rgb_writer->SetFileName(outputFileName);
		rgb_writer->Update();
	}
	
} // end of namespace lsrm
#endif
