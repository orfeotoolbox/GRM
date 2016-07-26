/*=========================================================================

  Program: Generic Region Merging Library
  Language: C++
  author: Lassalle Pierre
  contact: lassallepierre34@gmail.com



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef GRM_GRAPH_TO_OTBIMAGE_H
#define GRM_GRAPH_TO_OTBIMAGE_H
#include <itkRGBPixel.h>
#include <itkImageRegion.h>
#include <otbImage.h>
#include <otbVectorImage.h>
#include <otbImageFileReader.h>
#include <otbImageFileWriter.h>
#include "grmGraph.h"
#include <string>
#include <stdlib.h>
#include <time.h>
#include "lpContour.h"

namespace grm
{
	template<class TGraph>
	class GraphToOtbImage
	{
	public:
		
		/* Some convenient typedefs */
		typedef TGraph GraphType;
		typedef typename GraphType::NodeType NodeType;
		typedef std::vector< std::shared_ptr<NodeType> > NodeList;
		typedef typename NodeList::const_iterator NodeConstIterator;
		typedef unsigned int LabelPixelType;
		typedef otb::Image<LabelPixelType, 2> LabelImageType;
		typedef unsigned char ClusterPixelType;
		typedef otb::VectorImage<ClusterPixelType, 2> ClusteredImageType;
		using ContourOperator = lp::ContourOperations;
		

		LabelImageType::Pointer GetLabelImage(const GraphType& graph,
											  const unsigned int width,
											  const unsigned int height);

		ClusteredImageType::Pointer GetClusteredOutput(const GraphType& graph,
													   const unsigned int width,
													   const unsigned int height);	
	};
	
} // end of namespace grm
#include "grmGraphToOtbImage.txx"
#endif
