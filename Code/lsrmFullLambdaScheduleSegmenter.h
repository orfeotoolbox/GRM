#ifndef __LSRM_FULL_LAMBDA_SCHEDULE_SEGMENTER_H
#define __LSRM_FULL_LAMBDA_SCHEDULE_SEGMENTER_H
#include "lsrmSegmenter.h"

namespace lsrm
{
	struct FLSNode : Node<FLSNode>
	{
		std::vector<float> m_Means;
	};

	struct FLSParam{};

	template<class TImage>
	class FullLambdaScheduleSegmenter : public Segmenter< TImage, FLSNode, FLSParam>
	{
	public:
		/* Some convenient typedefs */
		typedef Segmenter<TImage, FLSNode, FLSParam> Superclass;
		typedef TImage ImageType;
		typedef typename Superclass::GraphType GraphType;
		typedef typename Superclass::NodePointerType NodePointerType;
		typedef typename Superclass::GraphOperatorType GraphOperatorType;
		typedef GraphToOtbImage<GraphType> IOType;

		FullLambdaScheduleSegmenter();

		void Update();

		float ComputeMergingCost(NodePointerType n1, NodePointerType n2);
		void UpdateSpecificAttributes(NodePointerType n1, NodePointerType n2);
		void InitFromImage();
	};
} // end of namespace lsrm
#include "lsrmFullLambdaScheduleSegmenter.txx"
#endif

