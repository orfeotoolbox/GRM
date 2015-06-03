#ifndef __LSRM_GRAPH_TO_OTBIMAGE_H
#define __LSRM_GRAPH_TO_OTBIMAGE_H
#include <itkRGBPixel.h>
#include <itkImageRegion.h>
#include <otbImage.h>
#include <otbVectorImage.h>
#include <otbImageFileReader.h>
#include <otbImageFileWriter.h>
#include "lsrmGraph.h"
#include <string>
#include <stdlib.h>
#include <time.h>

namespace lsrm
{
	template<class TGraph>
	class GraphToOtbImage
	{
	public:
		
		/* Some convenient typedefs */
		typedef TGraph GraphType;
		typedef typename GraphType::NodeType NodeType;
		typedef std::vector< boost::shared_ptr<NodeType> > NodeList;
		typedef typename NodeList::const_iterator NodeConstIterator;
		typedef unsigned long int LabelPixelType;
		typedef otb::Image<LabelPixelType, 2> LabelImageType;
		typedef unsigned char ClusterPixelType;
		typedef otb::VectorImage<ClusterPixelType, 2> ClusteredImageType;
		

		LabelImageType::Pointer GetLabelImage(const GraphType& graph,
											  const unsigned int width,
											  const unsigned int height);

		ClusteredImageType::Pointer GetClusteredOutput(const GraphType& graph,
													   const unsigned int width,
													   const unsigned int height);	
	};
	
} // end of namespace lsrm
#include "lsrmGraphToOtbImage.txx"
#endif
