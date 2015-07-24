set(DOCUMENTATION "GRM OTB Application for region merging segmentation of very high resolution satellite scenes.")

# define the dependencies of the include module and the tests
otb_module(otbGRM
	DEPENDS
	OTBCommon
		OTBApplicationEngine
		OTBStreaming
		OTBExtendedFilename
		OTBImageIO
	TEST_DEPENDS
		OTBTestKernel
		OTBCommandLine
	DESCRIPTION
		"${DOCUMENTATION}"
)
