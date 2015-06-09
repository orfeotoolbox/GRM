#ifndef __LSRM_SPRING_SEGMENTER_H
#define __LSRM_SPRING_SEGMENTER_H
#include "lsrmSegmenter.h"

namespace lsrm
{
	struct SpringNode : Node<SpringNode>
	{
		std::vector<float> m_Means;
	};

	struct SpringParam{};

	template<class TImage>
	class SpringSegmenter : public Segmenter< TImage, SpringNode, SpringParam>
	{
	public:
		/* Some convenient typedefs */
		typedef Segmenter<TImage, SpringNode, SpringParam> Superclass;
		typedef TImage ImageType;
		typedef typename Superclass::GraphType GraphType;
		typedef typename Superclass::NodePointerType NodePointerType;
		typedef typename Superclass::GraphOperatorType GraphOperatorType;
		typedef GraphToOtbImage<GraphType> IOType;

		SpringSegmenter();

		void Update();

		float ComputeMergingCost(NodePointerType n1, NodePointerType n2);
		void UpdateSpecificAttributes(NodePointerType n1, NodePointerType n2);
		void InitFromImage();
	};
} // end of namespace lsrm
#include "lsrmSpringSegmenter.txx"
#endif
