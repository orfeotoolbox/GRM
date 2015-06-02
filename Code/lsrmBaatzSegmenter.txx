#ifndef __LSRM_BAATZ_SEGMENTER_TXX
#define __LSRM_BAATZ_SEGMENTER_TXX
#include <otbImageFileReader.h>
#include <itkImageRegionIterator.h>

namespace lsrm
{

	template<class TImage>
	BaatzSegmenter<TImage>::BaatzSegmenter() : Superclass()
	{
		this->m_DoBFSegmentation = true;
		this->m_NumberOfIterations = 75;
	}
	
	template<class TImage>
	void
	BaatzSegmenter<TImage>::InitFromImage()
	{
		typedef otb::ImageFileReader<TImage> ImageReaderType;
		typedef itk::ImageRegionIterator<TImage> ImageIterator;

		typename ImageReaderType::Pointer reader = ImageReaderType::New();
		reader->SetFileName(this->m_InputFileName);
		reader->Update();

		this->m_ImageWidth = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
		this->m_ImageHeight = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
		this->m_NumberOfComponentsPerPixel = reader->GetOutput()->GetNumberOfComponentsPerPixel();

		std::size_t idx = 0;
		ImageIterator it(reader->GetOutput(), reader->GetOutput()->GetLargestPossibleRegion());
		for(it.GoToBegin(); !it.IsAtEnd(); ++it)
		{
			this->m_Graph.m_Nodes[idx]->m_Means.reserve(this->m_NumberOfComponentsPerPixel);
			this->m_Graph.m_Nodes[idx]->m_SquareMeans.reserve(this->m_NumberOfComponentsPerPixel);
			this->m_Graph.m_Nodes[idx]->m_SpectralSum.reserve(this->m_NumberOfComponentsPerPixel);
			this->m_Graph.m_Nodes[idx]->m_Std.assign(this->m_NumberOfComponentsPerPixel, 0.0f);

			for(std::size_t b = 0; b < this->m_NumberOfComponentsPerPixel; ++b)
			{
				this->m_Graph.m_Nodes[idx]->m_Means.push_back(it.Get()[b]);
				this->m_Graph.m_Nodes[idx]->m_SquareMeans.push_back((it.Get()[b])*(it.Get()[b]));
				this->m_Graph.m_Nodes[idx]->m_SpectralSum.push_back(it.Get()[b]);
			}	
			++idx;
		}
	}

	template<class TImage>
	float
	BaatzSegmenter<TImage>::ComputeMergingCost(NodePointerType n1, NodePointerType n2)
	{
		const unsigned int a1 = n1->m_Area, a2 = n2->m_Area, a_sum = a1 + a2;

		float spect_cost = 0.0f;
		float mean, square_mean, sum, std;

		for (unsigned int b = 0; b < this->m_NumberOfComponentsPerPixel; b++)
		{
			mean = ((a1 * n1->m_Means[b]) + (a2 * n2->m_Means[b])) / a_sum;
			square_mean = n1->m_SquareMeans[b] + n2->m_SquareMeans[b];
			sum = n1->m_SpectralSum[b] + n2->m_SpectralSum[b];
			std = std::sqrt((square_mean - 2*mean*sum + a_sum * mean* mean) / a_sum);
			spect_cost += (a_sum * std - a1 * n1->m_Std[b] - a2 * n2->m_Std[b]);
		}
		spect_cost *= this->m_Param.m_ShapeWeight;

		if(spect_cost < this->m_Threshold)
		{
			float shape_cost, smooth_f, compact_f;

 			// Compute the shape merging cost
			const float p1 = static_cast<float>(n1->m_Perimeter);
			const float p2 = static_cast<float>(n2->m_Perimeter);
			const unsigned int boundary = (GraphOperatorType::FindEdge(n1, n2))->m_Boundary;
			const float p3 = p1 + p2 - 2 * static_cast<float>(boundary);
			
			const BoundingBox merged_bbox = MergeBoundingBoxes(n1->m_Bbox, n2->m_Bbox);
			const float bb1_perimeter = static_cast<float>(2*n1->m_Bbox.m_W + 2*n1->m_Bbox.m_H);
			const float bb2_perimeter = static_cast<float>(2*n2->m_Bbox.m_W + 2*n2->m_Bbox.m_H);
			const float mbb_perimeter = static_cast<float>(2 * merged_bbox.m_W + 2 * merged_bbox.m_H);

			smooth_f = a_sum*p3/mbb_perimeter - a1*p1/bb1_perimeter - a2*p2/bb2_perimeter;
			compact_f = a_sum*p3/std::sqrt(a_sum) - a1*p1/std::sqrt(a1) - a2*p2/std::sqrt(a2);

			shape_cost = this->m_Param.m_ShapeWeight * compact_f + (1-this->m_Param.m_ShapeWeight) * smooth_f;

			return (spect_cost + (1-this->m_Param.m_ShapeWeight)*shape_cost);
		}
		else
			return spect_cost;
	}

	template<class TImage>
	void
	BaatzSegmenter<TImage>::UpdateSpecificAttributes(NodePointerType n1, NodePointerType n2)
	{
		const float a1 = static_cast<float>(n1->m_Area);
		const float a2 = static_cast<float>(n2->m_Area);
		const float a_sum = a1 + a2;

		for(unsigned int b = 0; b < this->m_NumberOfComponentsPerPixel; ++b)
		{
			n1->m_Means[b] = (a1 * n1->m_Means[b] + a2 * n2->m_Means[b]) / a_sum;
			n1->m_SquareMeans[b] += n2->m_SquareMeans[b];
			n1->m_SpectralSum[b] += n2->m_SpectralSum[b];
			n1->m_Std[b] = std::sqrt((n1->m_SquareMeans[b] - 2 * n1->m_Means[b] * n1->m_SpectralSum[b] +
									  a_sum * n1->m_Means[b] * n1->m_Means[b]) / a_sum);
		}
	}

	template<class TImage>
	void
	BaatzSegmenter<TImage>::RunSegmentation()
	{
		GraphOperatorType::InitNodes(this->m_Graph, *this, this->m_InputFileName, FOUR);
		
		bool prev_merged =
			GraphOperatorType::PerfomAllIterationsWithLMBFAndConstThreshold(this->m_Graph, *this,
																			this->m_Threshold, this->m_NumberOfIterations,
																			this->m_ImageWidth, this->m_ImageHeight);

		if(prev_merged && this->m_DoBFSegmentation)
		{
			prev_merged =
				GraphOperatorType::PerfomAllIterationsWithBFAndConstThreshold(this->m_Graph, *this,
																			  this->m_Threshold, this->m_NumberOfIterations,
																			  this->m_ImageWidth, this->m_ImageHeight);
		}

		if(!this->m_ClusteredImageFileName.empty())
			IOType::WriteOutputRGBImage(this->m_Graph,
										this->m_ImageWidth,
										this->m_ImageHeight,
										this->m_ClusteredImageFileName);

		if(!this->m_LabelImageFileName.empty())
			IOType::WriteLabelImage(this->m_Graph,
									this->m_ImageWidth,
									this->m_ImageHeight,
									this->m_LabelImageFileName);
	}
} // end of namespace lsrm

#endif







