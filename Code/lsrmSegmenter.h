#ifndef __LSRM_SEGMENTER_H
#define __LSRM_SEGMENTER_H
#include "macro-generator.h"
#include "lsrmGraph.h"
#include "lsrmGraphOperations.h"

namespace lsrm
{
	template<class TImage, class TNode, class TParam>
	class Segmenter
	{
	public:

		/* Some convenient typedefs */
		
		typedef Segmenter<TImage, TNode, TParam> Self;
		typedef TImage ImageType;
		typedef TNode NodeType;
		typedef TParam ParamType;
		typedef lsrm::Graph<NodeType> GraphType;
		typedef typename GraphType::EdgeType EdgeType;
		typedef GraphOperations<Self> GraphOperatorType;
		typedef typename GraphOperatorType::NodePointerType NodePointerType;

		/* Default constructor and destructor */
		
		Segmenter(){};
		~Segmenter(){};

		/*
		 * This method performs the region merging segmentation.
		 */
		virtual void RunSegmentation() = 0;

		/* methods to overload */

		/*
		 * Given 2 smart adjacent node pointers (boost::shared_ptr), this
		 * method has to compute the merging cost which is coded as a float.
		 *
		 * @params
		 * NodePointerType n1 : Smart pointer to node 1
		 * NodePointerType n2 : Smart pointer to node 2
		 *
		 * @return the merging cost.
		 */
		virtual float ComputeMergingCost(NodePointerType n1, NodePointerType n2) = 0;

		/*
		 * Given 2 smart adjacent node pointers (boost::shared_ptr), this
		 * method merges th node n2 into the node n1 by updating the customized
		 * attributes of the node n1.
		 *
		 * @params
		 * NodePointerType n1 : Smart pointer to node 1
		 * NodePointerType n2 : Smart pointer to node 2
		 *
		 */
		virtual void UpdateSpecificAttributes(NodePointerType n1, NodePointerType n2) = 0;

		/*
		 * Given the input image, this method initializes the
		 * internal and specific attributes of the graph.
		 *
		 * @params
		 * const std::string& inputFileName : input image path
		 */
		virtual void InitFromImage() = 0;
		
		/* Set methods */
		SetMacro(bool, DoBFSegmentation);
		SetMacro(unsigned int, NumberOfIterations);
		SetMacro(float, Threshold);
		SetMacro(ParamType, Param);
		SetMacro(std::string, InputFileName);
		SetMacro(std::string, ClusteredImageFileName);
		SetMacro(std::string, LabelImageFileName);
		SetMacro(std::string, ContourImageFileName);

	protected:

		/* Activate the Best Fitting heuristic */
		bool m_DoBFSegmentation;
		
		/* Number of iterations for the Local Mutual Best Fitting segmentation */
		unsigned int m_NumberOfIterations;

		/* Limit threshold for the region merging criterion  */
		float m_Threshold;

		/* Specific parameters required for the region merging criterion */
		ParamType m_Param;

		/* Graph */
		GraphType m_Graph;

		/* Image information (has to be initialized in the method InitFromImage) */
		unsigned int m_ImageWidth; // Number of columns
		unsigned int m_ImageHeight; // NUmber of rows
		unsigned int m_NumberOfComponentsPerPixel; // Number of spectral bands
		std::string m_InputFileName;
		std::string m_ClusteredImageFileName;
		std::string m_LabelImageFileName;
		std::string m_ContourImageFileName;
	};
} // end of namespace lsrm

#endif




