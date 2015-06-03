#ifndef __LSRM_BAATZ_SEGMENTER_H
#define __LSRM_BAATZ_SEGMENTER_H
#include "lsrmSegmenter.h"
/*
 * Tutorial : Implementation of the Baatz & Schape criterion
 *
 * Details about the criterion can be found in the following publication:
 *
 * Martin Baatz and Arno Schape. Multiresolution segmentation: an optimization approach for high quality multi-scale image segmentation.
 * Angewandte Geographische Informationsverarbeitung XII, pages 12–23, 2000.
 *
 * The steps are ordered has to be followed in a chronological way for a full understanding.
 * This tutorial does not aim at explaining all the details of the large scale region merging
 * but gives all the elements to use it properly.
 */

namespace lsrm
{
	/*
	 * Step 1 :
	 * We define the specific attributes required for the Baatz & Schape criterion.
	 * Regions are represented by nodes in a graph and the lsrm library has an internal
	 * representation of the nodes with the class lsrm::Node.
	 * --> A node contains a unique number to be indentified, it corresponds to the vectorized
	 *     coordinates of its first pixel and its name is m_Id.
	 *     To retrieve the 2D coordinates from the value of m_Id, it is necessary to know the dimension
	 *     of the image (the number of rows and columns), then :
	 *     x = m_Id % columns and y = m_Id / columns.
	 *
	 * --> A node contains the perimeter (m_Perimeter) of its corresponding region, it is the length of the external
	 *     boundary of the region. For example a region of one pixel has a perimeter of 4.
	 *
	 * --> A node contains the area (m_Area) of its corresponding region, it is the number of pixels within the
	 *     region.
	 *
	 * --> A node contains the minimum rectangular bounding box (parrallel to the image axis) of the region (m_Bbox).
	 *     the boundix box is determined by the structure lsrm::BoundingBox with the coordinates of the upper left
	 *     corner pixel and the width and the height (m_UX, m_UY, m_W, m_H).
	 *
	 * After Reading the paper about the Baatz & Schape criterion, we can conclude that we have to define additional
	 * spectral attributes for a node: its mean vector, the square mean vector, the sum of all the pixels
	 * and the standard deviation vector.
	 *
	 * We define a new class BaatzNode which inherits from the template structure Node. Notice that the template
	 * is the derived node type, then the template type is BaatzNode.
	 */
	struct BaatzNode : Node<BaatzNode>
	{
		std::vector<float> m_Means;
		std::vector<float> m_SquareMeans;
		std::vector<float> m_SpectralSum;
		std::vector<float> m_Std;
	};

	/*
	 * Step 2
	 *
	 * The Baatz & Schape criterion has user-defined parameters, the spectral weight
	 * which is the relative importance given to the spectral components and the shape
	 * weight for the shape components.
	 *
	 * We define then a structure wrapping these two parameters we called BaatzParam.
	 */
	struct BaatzParam
	{
		float m_SpectralWeight;
		float m_ShapeWeight;
	};

	/*
	 * Step 3 : 
	 *
	 */
	
	template<class TImage>
	class BaatzSegmenter : public Segmenter< TImage, BaatzNode, BaatzParam>
	{
	public:

		/* Some convenient typedefs */
		typedef Segmenter<TImage, BaatzNode, BaatzParam> Superclass;
		typedef TImage ImageType;
		typedef typename Superclass::GraphType GraphType;
		typedef typename Superclass::NodePointerType NodePointerType;
		typedef typename Superclass::GraphOperatorType GraphOperatorType;
		typedef GraphToOtbImage<GraphType> IOType;

		BaatzSegmenter();
		
		void Update();
		float ComputeMergingCost(NodePointerType n1, NodePointerType n2);
		void UpdateSpecificAttributes(NodePointerType n1, NodePointerType n2);
		void InitFromImage();
	};
	
} // end of namespace lsrm
#include "lsrmBaatzSegmenter.txx"
#endif



