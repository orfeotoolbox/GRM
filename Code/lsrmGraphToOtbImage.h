#ifndef __LSRM_GRAPH_TO_OTBIMAGE_H
#define __LSRM_GRAPH_TO_OTBIMAGE_H
#include <itkRGBPixel.h>
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

		/*
		 * Given a graph of nodes and the size of the image, it returns
		 * a raster image where to every pixel of a region is assigned
		 * the same unique label value.
		 *
		 * @params
		 * const GraphType& graph : graph of nodes
		 * const unsigned int width : width of the output labeled image.
		 * const unsigned int height : height of the output labeled image.
		 * const std::string& outputFileName : filename of the output image.
		 */
		static void WriteLabelImage(const GraphType& graph,
									const unsigned int width,
									const unsigned int height,
									const std::string& outputFileName);

		/*
		 * Given a graph of nodes and the size of the image, it returns
		 * a raster image where to every pixel of a region is assigned
		 * the same rgb color.
		 *
		 * @params
		 * const GraphType& graph : graph of nodes
		 * const unsigned int width : width of the output rgb image.
		 * const unsigned int height : height of the output rgb image.
		 * const std::string& outputFileName : filename of the output image.
		 */
		static void WriteOutputRGBImage(const GraphType& graph,
										const unsigned int width,
										const unsigned int height,
										const std::string& outputFileName);		
	};
	
} // end of namespace lsrm
#include "lsrmGraphToOtbImage.txx"
#endif
