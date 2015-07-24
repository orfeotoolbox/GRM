/*=========================================================================

  Program: Generic Region Merging Library
  Language: C++
  author: Lassalle Pierre
  contact: lassallepierre34@gmail.com



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

     Template Model to help the developper to add a new criterion
     This file as well as his corresponding txx file has to be added
     src folder.

=========================================================================*/
#ifndef __GRM_MYSEGMENTER_TXX
#define __GRM_MYSEGMENTER_TXX
#include <otbImageFileReader.h>
#include <itkImageRegionIterator.h>

/*
	Important to read:
	
	The graph of segments can be accessed like this: this->m_Graph
	The number of segments in the graph: this->m_Graph.size()
	To access to the segment pointer at the position idx: this->m_Graph.m_Nodes[idx]
	For each segment pointer you can used the following attributes:
		this->m_Graph.m_Nodes[idx]->m_Perimeter // Perimeter of the segment.
		this->m_Graph.m_Nodes[idx]->m_Area // Number of pixels contained in the segment.
		this->m_Graph.m_Nodes[idx]->m_Bbox // Bounding box of the segment:
			this->m_Graph.m_Nodes[idx]->m_Bbox.m_UX : upper left x coordinate
			this->m_Graph.m_Nodes[idx]->m_Bbox.m_UY : upper left y coordinate
			this->m_Graph.m_Nodes[idx]->m_Bbox.m_W : width
			this->m_Graph.m_Nodes[idx]->m_Bbox.m_UX : height
		this->m_Graph.m_Nodes[idx]->m_Contour: moves along the contour of the segment:
			To generate the set of the coordinates of the border pixels:
				lp::CellLists borderPixels; // (std::unordered_set<long unsigned int>)
				lp::ContourOperations::GenerateBorderCells(borderPixels, this->m_Graph.m_Nodes[idx]->m_Contour, this->m_Graph.m_Nodes[idx]->m_Id, this->m_ImageWidth);
		this->m_Graph.m_Nodes[idx]->m_Edges // std::vector<EdgeType> containing edges to adjacent segment
			Given an edge e:
				e.GetRegion() : access to the adjacent segment pointer.
				e.m_Boundary :  lenght of the border between the current segment and the adjacent segment.
				e.m_Cost: merging cost between the current segment and the adjacent segment.
*/

namespace grm
{

	template<class TImage>
	void
	MySegmenter<TImage>::InitFromImage()
	{
		typedef itk::ImageRegionIterator<TImage> ImageIterator;

		this->m_ImageWidth = this->m_InputImage->GetLargestPossibleRegion().GetSize()[0];
		this->m_ImageHeight =this->m_InputImage->GetLargestPossibleRegion().GetSize()[1];
		this->m_NumberOfComponentsPerPixel = this->m_InputImage->GetNumberOfComponentsPerPixel();

		std::size_t idx = 0;
		ImageIterator it(this->m_InputImage, this->m_InputImage->GetLargestPossibleRegion());
		for(it.GoToBegin(); !it.IsAtEnd(); ++it)
		{
			// Initialize your specific attributes for each pixel of the image.

			for(std::size_t b = 0; b < this->m_NumberOfComponentsPerPixel; ++b)
			{
				// Initialize your specific attributes if they depend on spectral values of pixels.
			}	
			++idx;
		}
	}

	template<class TImage>
	float
	MySegmenter<TImage>::ComputeMergingCost(NodePointerType n1, NodePointerType n2)
	{
		// Determine how to compute the merging cost between the two adjacent segments.
	}

	template<class TImage>
	void
	MySegmenter<TImage>::UpdateSpecificAttributes(NodePointerType n1, NodePointerType n2)
	{
		// Determine how to update your specif attributes when segment n2 is merged into segment n1.
		// This means that you have to update the specific attributes of segment n1 according to
		// the specific attributes of segments n1 and n2.
	}

} // end of namespace grm

#endif
