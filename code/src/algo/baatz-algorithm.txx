/*=========================================================================

  Program: Implementation of the Baatz & Schäpe criterion using the GRM library
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __BAATZ_ALGORITHM_TXX
#define __BAATZ_ALGORITHM_TXX

#include "baatz-algorithm.h"
#include <otbVectorImage.h>
#include <otbImageFileReader.h>
#include <itkImageRegionIterator.h>

template<class TInputImage>
void 
BaatzAlgorithmRM<TInputImage>::InitFromImage()
{
	typedef otb::ImageFileReader<TInputImage> InputImageFileReaderType;
	typedef itk::ImageRegionIterator<TInputImage> InputImageIteratorType;

	// Read the input image
	auto input_reader = InputImageFileReaderType::New();
	input_reader->SetFileName(this->m_Input);
	input_reader->Update();

	// Retrieve useful information from the input image
	auto regionToProcess = input_reader->GetOutput()->GetLargestPossibleRegion();
	unsigned int rows = regionToProcess.GetSize()[1];
	unsigned int cols = regionToProcess.GetSize()[0];
	unsigned int bands = input_reader->GetOutput()->GetNumberOfComponentsPerPixel();

	// Step 1: For efficiency the user is advised to allocate the initial number of regions
	this->m_RegionList.reserve(rows*cols);

	// Step 2: the user has to intialize the specific attributes of the initial regions
	InputImageIteratorType it(input_reader->GetOutput(), regionToProcess);
	for(it.GoToBegin(); !it.IsAtEnd(); ++it)
	{
		auto new_region = std::make_shared<BaatzRegion>();
		new_region->m_Area = 1;
		new_region->m_Perimeter = 4;
		new_region->m_Means.reserve(bands);
		new_region->m_SquareMeans.reserve(bands);
		new_region->m_SpectralSum.reserve(bands);
		new_region->m_Std.reserve(bands);
		for(unsigned int b = 0; b<bands; ++b)
		{
			new_region->m_Means.push_back(it.Get()[b]);
			new_region->m_SquareMeans.push_back(pow(it.Get()[b], 2));
			new_region->m_SpectralSum.push_back(it.Get()[b]);
			new_region->m_Std.push_back(0);
		}
		// The user has to push the initial regions
		this->m_RegionList.push_back(new_region);
	}

	// Step 3: The neighborhhod and some internal attributes have to be initialized
	// the user has to call the method InitRegions
	this->InternalInit(rows, cols);
}

// Let assume that regions r1 and r2 have to be merged and r2 is merged into r1
// The user has to describe how the specific attributes of r1 have to be updated
// by defining the mandatory method UpdateAttribute(RegionPointerType, RegionPointerType)
template<class TInputImage>
void 
BaatzAlgorithmRM<TInputImage>::UpdateAttribute(RegionPointerType r1, RegionPointerType r2)
{
	float a1 = r1->m_Area, a2 = r2->m_Area, a_tot = a1 + a2;
	for(unsigned int b=0; b<r1->m_Means.size(); ++b)
	{
		r1->m_Means[b] = ((a1*r1->m_Means[b]) + (a2*r2->m_Means[b])) / a_tot;
		r1->m_SquareMeans[b] += r2->m_SquareMeans[b];
		r1->m_SpectralSum[b] += r2->m_SpectralSum[b];
		r1->m_Std[b] = std::sqrt((r1->m_SquareMeans[b] - 
								2*r1->m_Means[b]*r1->m_SpectralSum[b] + 
								a_tot*r1->m_Means[b]*r1->m_Means[b]) / a_tot); 
	}
	r1->m_Area += r2->m_Area;
	r1->m_Perimeter += r2->m_Perimeter - 2 * r1->GetConnections(r2);
}

// This method is obviously mandatory but its implementation depends on some
// subtilities of your algorithm
// What is common is the the iterative merging process of the region, what can be different
// is the stopping criterion and the behavior of the threshold
template<class TInputImage>
void 
BaatzAlgorithmRM<TInputImage>::Segmentation()
{
	bool prev_merged = true;
	float threshold = this->m_Parameters.m_Scale*this->m_Parameters.m_Scale;
	float spec_w = this->m_Parameters.m_SpectralWeight;
	float shap_w = this->m_Parameters.m_ShapeWeight;

	MergingCostFunction cost_func = [&](RegionPointerType r1, RegionPointerType r2)->double
	{
		unsigned int bands = r1->m_Means.size();
		float a1 = r1->m_Area, a2 = r2->m_Area, a_tot = a1 + a2;

		// Compute the spectral merging cost
		double spec_cost = 0.0;
		float mean, square_mean, sum, std, tmp_cost;
		for(unsigned int b = 0; b < bands; b++)
		{
			mean = ((a1*r1->m_Means[b]) + (a2*r2->m_Means[b])) / a_tot;
			square_mean = r1->m_SquareMeans[b] + r2->m_SquareMeans[b];
			sum = r1->m_SpectralSum[b] + r2->m_SpectralSum[b];
			std = std::sqrt((square_mean - 2*mean*sum + a_tot*mean*mean)/a_tot);
			tmp_cost = a_tot * std - a1*r1->m_Std[b] - a2*r2->m_Std[b];
			spec_cost += tmp_cost;
		}
		spec_cost *= spec_w;

		if(spec_cost < threshold)
		{
			double shape_cost, smooth_f, compact_f;

			// Compute the shape merging cost
			float p1 = r1->m_Perimeter, p2 = r2->m_Perimeter, 
					p3 = p1 + p2 - 2*r1->GetConnections(r2);

			auto mmbox = r1->GetResultingBbox(r2);
			float bb1 = 2*r1->GetBboxSize(0) + 2*r1->GetBboxSize(1),
				bb2 = 2*r2->GetBboxSize(0) + 2*r2->GetBboxSize(1),
				bb3 = 2*mmbox[2] + 3*mmbox[3];

			smooth_f = a_tot*p3/bb3 - a1*p1/bb1 - a2*p2/bb2;
			compact_f = a_tot*p3/std::sqrt(a_tot) - a1*p1/std::sqrt(a1) - a2*p2/std::sqrt(a2);

			shape_cost = shap_w * compact_f + (1-shap_w) * smooth_f;

			return (spec_cost + (1-spec_w)*shape_cost);
		}
		else
			return spec_cost;
	};

	while(prev_merged)
	{
		std::cout << "." << std::flush;
		prev_merged = false;

		this->m_RMHandler.UpdateMergingCosts(this->m_RegionList, cost_func);

		// Do a loop on the regions of the graph
		for(auto& r : this->m_RegionList)
		{
			// For each explored region, we determine if the LMBF is met
			auto ref_region = this->m_RMHandler.CheckLMBF(r, threshold);
			// If it holds then the pointer to ref_region is not null
			if(ref_region != nullptr)
			{
				// Process to merge the regions
				// Step 1: Merge the specific attributes
				UpdateAttribute(ref_region, ref_region->GetClosestNeighbor());
				// Step 2: Internal update (mandatory)
				this->m_RMHandler.Update(ref_region, ref_region->GetClosestNeighbor());

				prev_merged = true;
			}
		}

		// After one iteration, you have to remove the expired regions, i.e. regions
		// who have merged into a larger one
		this->m_RMHandler.RemoveExpiredVertices(this->m_RegionList);
	}
	std::cout << "\n";
}

#endif
