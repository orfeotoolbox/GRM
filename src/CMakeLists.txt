set(OTBGRM_SRC
	grmNeighborhood.cxx
	lpContour.cxx
)

add_library(OTBGRM ${OTBGRM_SRC})
target_link_libraries(OTBGRM ${OTBCommon_LIBRARIES})

otb_module_target(OTBGRM)
