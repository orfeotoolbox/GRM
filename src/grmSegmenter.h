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
#ifndef __GRM_SEGMENTER_H
#define __GRM_SEGMENTER_H
#include "grmMacroGenerator.h"
#include "grmGraphOperations.h"
#include "grmGraphToOtbImage.h"

namespace grm
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
		typedef Graph<NodeType> GraphType;
		typedef typename GraphType::EdgeType EdgeType;
		typedef GraphOperations<Self> GraphOperatorType;
		typedef typename GraphOperatorType::NodePointerType NodePointerType;
		typedef GraphToOtbImage<GraphType> IOType;
		typedef typename IOType::LabelImageType LabelImageType;
		typedef typename IOType::ClusteredImageType ClusteredImageType;

		/* Default constructor and destructor */
		
		Segmenter(){
			this->m_DoFastSegmentation = false;
			this->m_NumberOfIterations = 0;
			this->m_Complete = false;
		};
		~Segmenter(){};

		/*
		 * This method performs the region merging segmentation.
		 */
		virtual void Update()
		{
			GraphOperatorType::InitNodes(this->m_InputImage, *this, FOUR);
			bool prev_merged = false;

			if(this->m_DoFastSegmentation)
			{
				prev_merged = GraphOperatorType::PerfomAllDitheredIterationsWithBF(*this);
			}
			else
			{
				prev_merged = GraphOperatorType::PerfomAllIterationsWithLMBFAndConstThreshold(*this);
			}

			this->m_Complete = !prev_merged;
		}

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

		/* Return the label image */
		inline typename LabelImageType::Pointer GetLabeledClusteredOutput()
			{
				IOType io;
				auto labelImg = io.GetLabelImage(this->m_Graph, this->m_ImageWidth, this->m_ImageHeight);
				return labelImg;
			}

		inline typename ClusteredImageType::Pointer GetClusteredImageOutput()
			{
				IOType io;
				auto clusteredImg = io.GetClusteredOutput(this->m_Graph, this->m_ImageWidth, this->m_ImageHeight);
				return clusteredImg;
			}
		
		/* Set methods */
		GRMSetMacro(bool, DoFastSegmentation);
		GRMSetMacro(unsigned int, NumberOfIterations);
		GRMSetMacro(float, Threshold);
		GRMSetMacro(ParamType, Param);
		GRMSetMacro(unsigned int, ImageWidth);
		GRMSetMacro(unsigned int, ImageHeight);
		GRMSetMacro(unsigned int, NumberOfComponentsPerPixel);
		inline void SetInput(TImage * in){ m_InputImage = in;}
		inline bool GetComplete(){ return this->m_Complete;}

		/* Get methods */
		GRMGetMacro(float, Threshold);
		GRMGetMacro(unsigned int, ImageWidth);
		GRMGetMacro(unsigned int, ImageHeight);
		GRMGetMacro(unsigned int, NumberOfComponentsPerPixel);
		GRMGetMacro(unsigned int, NumberOfIterations);
		
		/* Graph */
		GraphType m_Graph;
		
		
	protected:

		/* Boolean indicating if the segmentation procedure is achieved */
		bool m_Complete;

		/* Activate the fast segmentation */
		bool m_DoFastSegmentation;

		/* Number of iterations for the Local Mutual Best Fitting segmentation */
		unsigned int m_NumberOfIterations;

		/* Limit threshold for the region merging criterion  */
		float m_Threshold;

		/* Specific parameters required for the region merging criterion */
		ParamType m_Param;

		/* Image information (has to be initialized in the method InitFromImage) */
		unsigned int m_ImageWidth; // Number of columns
		unsigned int m_ImageHeight; // NUmber of rows
		unsigned int m_NumberOfComponentsPerPixel; // Number of spectral bands

		/* Pointer to the input image to segment */
		TImage * m_InputImage;
	};
} // end of namespace grm

#endif




