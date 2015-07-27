otb_module_test()

otb_test_application(NAME apGRM_BaatzCriterion
					APP otbGRM
					OPTIONS -in ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
					-out ${TEMP}/apGRMLabeledImage.tif int16
					-criterion bs
					-threshold 60
					-cw 0.7
					-sw 0.3
)

otb_test_application(NAME apGRM_EuclideanDistanceCriterion
					APP otbGRM
					OPTIONS -in ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
					-out ${TEMP}/apGRMLabeledImage.tif int16
					-criterion ed
					-threshold 30
)

otb_test_application(NAME apGRM_FullLambdaScheduleCriterion
					APP otbGRM
					OPTIONS -in ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
					-out ${TEMP}/apGRMLabeledImage.tif int16
					-criterion fls
					-threshold 500
)

otb_test_application(NAME apGRM_BaatzCriterionWithSpeed
					APP otbGRM
					OPTIONS -in ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
					-out ${TEMP}/apGRMLabeledImage.tif int16
					-speed 1
					-criterion bs
					-threshold 60
					-cw 0.7
					-sw 0.3
)

otb_test_application(NAME apGRM_EuclideanDistanceCriterionWithSpeed
					APP otbGRM
					OPTIONS -in ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
					-out ${TEMP}/apGRMLabeledImage.tif int16
					-speed 1
					-criterion ed
					-threshold 30
)

otb_test_application(NAME apGRM_FullLambdaScheduleCriterionWithSpeed
					APP otbGRM
					OPTIONS -in ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
					-out ${TEMP}/apGRMLabeledImage.tif int16
					-speed 1
					-criterion fls
					-threshold 500
)

otb_test_application(NAME apGRM_BaatzCriterionWithFixedNumberOfIterations
					APP otbGRM
					OPTIONS -in ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
					-out ${TEMP}/apGRMLabeledImage.tif int16
					-niter 75
					-criterion bs
					-threshold 60
					-cw 0.7
					-sw 0.3
)

otb_test_application(NAME apGRM_EuclideanDistanceCriterionWithFixedNumberOfIterations
					APP otbGRM
					OPTIONS -in ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
					-out ${TEMP}/apGRMLabeledImage.tif int16
					-niter 75
					-criterion ed
					-threshold 30
)

otb_test_application(NAME apGRM_FullLambdaScheduleCriterionWithFixedNumberOfIterations
					APP otbGRM
					OPTIONS -in ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
					-out ${TEMP}/apGRMLabeledImage.tif int16
					-niter 75
					-criterion fls
					-threshold 500
)
