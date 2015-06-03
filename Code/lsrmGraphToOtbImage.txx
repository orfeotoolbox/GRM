#ifndef __LSRM_GRAPH_TO_OTBIMAGE_TXX
#define __LSRM_GRAPH_TO_OTBIMAGE_TXX
#include "lsrmGraphToOtbImage.h"

namespace lsrm
{
	template<class TGraph>
	typename GraphToOtbImage<TGraph>::LabelImageType::Pointer
	GraphToOtbImage<TGraph>::GetLabelImage(const GraphType& graph,
										   const unsigned int width,
										   const unsigned int height)
	{
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

		return label_img;
	}

	template<class TGraph>
	typename GraphToOtbImage<TGraph>::ClusteredImageType::Pointer
	GraphToOtbImage<TGraph>::GetClusteredOutput(const GraphType& graph,
												const unsigned int width,
												const unsigned int height)
	{
		ClusteredImageType::IndexType index;
		ClusteredImageType::SizeType size;
		ClusteredImageType::RegionType region;

		index[0] = 0; index[1] = 0;
		size[0] = width; size[1] = height;
		region.SetIndex(index);
		region.SetSize(size);

		ClusteredImageType::Pointer clusterImg = ClusteredImageType::New();
		clusterImg->SetRegions(region);
		clusterImg->SetNumberOfComponentsPerPixel(3);
		clusterImg->Allocate();

		ClusteredImageType::PixelType pixelValue;
		pixelValue.Reserve(3);

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
				clusterImg->SetPixel(index, pixelValue);
			}
		}
		return clusterImg;
	}
		
} // end of namespace lsrm
#endif
