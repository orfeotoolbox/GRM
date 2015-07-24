set(${otb-module}_SRC
	grmBaatzSegmenter.txx
	grmFullLambdaScheduleSegmenter.txx
	grmGraphOperations.txx
	grmGraphToOtbImage.txx
	grmNeighborhood.cxx
	grmSpringSegmenter.txx
	lpContour.cxx
)

add_library(${otb-module} ${${otb-module}_SRC})
target_link_libraries(${otb_module} ${OTBCommon_LIBRARIES})

otb_module_target(${otb-module})
