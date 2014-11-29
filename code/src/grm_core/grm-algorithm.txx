/*=========================================================================

  Program: Generic Region Merging Library (GRM)
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __GRM_ALGORITHM_TXX
#define __GRM_ALGORITHM_TXX

#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "grm-algorithm.h"

namespace grm
{
	/*template<class TInputImage, class TRegion, class TParams>
	void
	RegionMergingAlgorithm<TInputImage, TRegion, TParams>::*/

	template<class TInputImage, class TRegion, class TParams>
	void
	RegionMergingAlgorithm<TInputImage, TRegion, TParams>::SetDimensionForEncoder(
															unsigned int rows,
															unsigned int columns)
	{
		m_RMHandler.SetDimensionForShapeEncoder(rows, columns);
	}

	template<class TInputImage, class TRegion, class TParams>
	void 
	RegionMergingAlgorithm<TInputImage, TRegion, TParams>::InternalInit(unsigned int rows, 
																		unsigned int columns)
	{
		m_RMHandler.SetDimensionForShapeEncoder(rows, columns);
		m_RMHandler.InitRegions(m_RegionList);
	}

	/*template<class TInputImage, class TRegion, class TParams>
	void
	RegionMergingAlgorithm<TInputImage, TRegion, TParams>::WriteLabelOutputImage()
	{
		assert(m_Input != "");
		assert(m_OutputLabel != "");

		typedef long unsigned int LabelPixelType;
		typedef otb::Image<LabelPixelType, 2> LabelImageType;
		typedef typename LabelImageType::IndexType LabelIndexType;
		typedef typename LabelImageType::SizeType LabelSizeType;
		typedef typename LabelImageType::RegionType LabelRegionType;

		typedef otb::ImageFileWriter<LabelImageType> LabelImageWriterType;

		// Allocate the image
		auto label_img = LabelImageType::New();
		LabelIndexType index; index[0] = 0; index[1] = 0;
		LabelSizeType size;
		auto dim = m_RMHandler.GetDimensionOfInputImage();
		size[0] = dim.first;
		size[1] = dim.second;
		assert(size[0] > 0);
		assert(size[1] > 0);
		LabelRegionType region; region.SetIndex(index); region.SetSize(size);
		label_img->SetRegions(region);
		label_img->Allocate();

		long unsigned int l = 1; // Lower value of the label
		std::for_each(m_RegionList.begin(), m_RegionList.end(), [&](RegionPointerType r){
			auto pixels = m_RMHandler.GenerateAllPixels(r);
			std::for_each(pixels.begin(), pixels.end(), [&](long unsigned int p){
				index[0] = p % size[0];
				index[1] = p / size[0];
				auto label_pix = label_img->GetPixel(index);
				label_img->SetPixel(index, label_pix);
			});
			++l;
		});

		auto label_writer = LabelImageWriterType::New();
		label_writer->SetFileName(m_OutputLabel);
		label_writer->SetInput(label_img);
		label_writer->Update();
	}*/

	template<class TInputImage, class TRegion, class TParams>
	void
	RegionMergingAlgorithm<TInputImage, TRegion, TParams>::WriteRGBOutputImage()
	{
		typedef unsigned char RGBPixelType;
		typedef otb::VectorImage<RGBPixelType, 2> RGBImageType;
		typedef typename RGBImageType::IndexType RGBIndexType;
		
		typedef otb::ImageFileReader<RGBImageType> RGBImageReaderType;
		typedef otb::ImageFileWriter<RGBImageType> RGBImageWriterType;

		auto rgb_reader = RGBImageReaderType::New();
		rgb_reader->SetFileName(m_Input);
		rgb_reader->Update();
		auto rgb_img = rgb_reader->GetOutput();
		RGBIndexType index;
	
		auto dim = m_RMHandler.GetDimensionOfInputImage();
		std::for_each(m_RegionList.begin(), m_RegionList.end(), [&](RegionPointerType r){
			auto pixels = m_RMHandler.GenerateFrontierPixels(r);
			std::for_each(pixels.begin(), pixels.end(), [&](long unsigned int p){
				index[0] = p % dim.first;
				index[1] = p / dim.first;
				auto rgb_pix = rgb_img->GetPixel(index);
				for(int b=0; b<rgb_img->GetNumberOfComponentsPerPixel(); b++)
					rgb_pix[b] = 0;
				rgb_img->SetPixel(index, rgb_pix);
			});
		});

		auto rgb_writer = RGBImageWriterType::New();
		rgb_writer->SetFileName(m_OutputRGB);
		rgb_writer->SetInput(rgb_img);
		rgb_writer->Update();
	}
} // end of namespace grm
#endif
