/*=======================================================================
 Program:   ORFEO Toolbox
 Language:  C++
 Date:      $Date$
 Version:   $Revision$
 
 Author: Lassalle Pierre
 Contact: lassallepierre34@gmail.com
 Website: http://pierre33.github.io/
 
 Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
 See OTBCopyright.txt for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =======================================================================*/
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "grmSpringSegmenter.h"
#include "grmFullLambdaScheduleSegmenter.h"
#include "grmBaatzSegmenter.h"
#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"

namespace otb
{
	namespace Wrapper
	{
		class GenericRegionMerging : public Application
		{
		public:
			typedef GenericRegionMerging Self;
			typedef Application SuperClass;
			typedef itk::SmartPointer<Self> Pointer;

			typedef FloatVectorImageType ImageType;
			typedef UInt32ImageType LabelImageType;

			itkNewMacro(Self);
			itkTypeMacro(GenericRegionMerging, otb::Application);

		private:

			void DoInit()
				{
					SetName("GenericRegionMerging");
					SetDescription("Segmentation using region merging algorithm");
          SetDocName("Generic Region Merging");
          SetDocLongDescription("This application allows to use the Generic "
            "Region Merging library (GRM).\n It provides currently 3 homogeneity "
            "criteria: Euclidean Distance, Full Lambda Schedule and Baatz & "
            "Schape criterion.");
          SetDocLimitations("This application is not adapted for large images.");
          SetDocAuthors("Pierre Lassalle");
          SetDocSeeAlso(" ");
          AddDocTag(Tags::Segmentation);

					AddParameter(ParameterType_InputImage, "in", "Input Image");
					AddParameter(ParameterType_OutputImage, "out", "Ouput Label Image");

					AddParameter(ParameterType_Choice, "criterion", "Homogeneity criterion to use");
					AddChoice("criterion.bs", "Baatz & Schape");
					AddChoice("criterion.ed", "Euclidean Distance");
					AddChoice("criterion.fls", "Full Lambda Schedule");

					AddParameter(ParameterType_Float, "threshold", "Threshold for the criterion");

					AddParameter(ParameterType_Int, "niter", "Number of iterations");
					SetDefaultParameterInt("niter", 0);
					MandatoryOff("niter");

					AddParameter(ParameterType_Int, "speed", "Activate it to boost the segmentation speed");
					SetDefaultParameterInt("speed", 0);
					MandatoryOff("speed");

					// For Baatz & Schape
					AddParameter(ParameterType_Float, "cw", "Weight for the spectral homogeneity");
					SetDefaultParameterFloat("cw", 0.5);
					MandatoryOff("cw");
					AddParameter(ParameterType_Float, "sw", "Weight for the spatial homogeneity");
					SetDefaultParameterFloat("sw", 0.5);
					MandatoryOff("sw");

          // Example
          SetDocExampleParameterValue("in", "QB_Toulouse_Ortho_XS.tif");
          SetDocExampleParameterValue("out", "labels.tif");
          SetDocExampleParameterValue("criterion", "bs");
          SetDocExampleParameterValue("threshold", "60");
					SetDocExampleParameterValue("cw", "0.7");
					SetDocExampleParameterValue("sw", "0.3");
				}

			void DoUpdateParameters()
				{
				}

			void DoExecute()
				{
					
					// Mandatory parameters
					// Input Image
					ImageType::Pointer image = GetParameterImage("in");
					image->Update();

					// Criterion selected
					const std::string selectedCriterion = GetParameterString("criterion");

					// Threshold
					float threshold = GetParameterFloat("threshold");

					const unsigned int niter = GetParameterInt("niter");
					const int speed = GetParameterInt("speed");

					// Output images
					LabelImageType::Pointer labelImage = LabelImageType::New();
					typedef otb::VectorImage<unsigned char, 2> RGBLabelImageType;
					RGBLabelImageType::Pointer rvbLabelImage = RGBLabelImageType::New();

					if(selectedCriterion == "bs")
					{
						const float cw = GetParameterFloat("cw");
						const float sw = GetParameterFloat("sw");

						grm::BaatzParam params;
						params.m_SpectralWeight = cw;
						params.m_ShapeWeight = sw;

						grm::BaatzSegmenter<ImageType> segmenter;
						segmenter.SetParam(params);
						segmenter.SetThreshold(threshold*threshold);
						segmenter.SetInput(image);

						if(niter > 0)
							segmenter.SetNumberOfIterations(niter);

						if(speed > 0)
							segmenter.SetDoFastSegmentation(true);

						segmenter.Update();

						
						labelImage = segmenter.GetLabeledClusteredOutput();
						//SetParameterOutputImage<LabelImageType>("out", segmenter.GetLabeledClusteredOutput());
					}
					else if(selectedCriterion == "ed")
					{
						grm::SpringSegmenter<ImageType> segmenter;
						segmenter.SetThreshold(threshold);
						segmenter.SetInput(image);
						if(niter > 0)
							segmenter.SetNumberOfIterations(niter);

						if(speed > 0)
							segmenter.SetDoFastSegmentation(true);

						segmenter.Update();

						labelImage = segmenter.GetLabeledClusteredOutput();
						//SetParameterOutputImage<LabelImageType>("out", segmenter.GetLabeledClusteredOutput());
					}
					else if(selectedCriterion == "fls")
					{
						grm::FullLambdaScheduleSegmenter<ImageType> segmenter;
						segmenter.SetThreshold(threshold);
						segmenter.SetInput(image);
						if(niter > 0)
							segmenter.SetNumberOfIterations(niter);

						if(speed > 0)
							segmenter.SetDoFastSegmentation(true);

						segmenter.Update();

						labelImage = segmenter.GetLabeledClusteredOutput();
						//SetParameterOutputImage<LabelImageType>("out", segmenter.GetLabeledClusteredOutput());
					}
					
					// Set output image projection, origin and spacing for labelImage
          labelImage->CopyInformation(image);
					SetParameterOutputImage<LabelImageType>("out", labelImage);
				}
		};
	} // end of namespace Wrapper
	
} // end of namespace otb

OTB_APPLICATION_EXPORT(otb::Wrapper::GenericRegionMerging)
