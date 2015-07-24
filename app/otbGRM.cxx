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

namespace otb
{
	namespace Wrapper
	{
		class otbGRM : public Application
		{
		public:
			typedef otbGRM Self;
			typedef itk::SmartPointer<Self> Pointer;

			typedef FloatVectorImageType ImageType;
			typedef UInt32ImageType LabelImageType;

			itkNewMacro(Self);
			itkTypeMacro(otbGRM, otb::Application);

		private:

			void DoInit()
				{
					SetName("otbGRM");
					SetDescription("This application allows to use the Generic Region Merging library (GRM) and provides currently 3 homogeneity criteria: Euclidean Distance, Full Lambda Schedule and Baatz & Schape criterion.");

					AddParameter(ParameterType_InputImage, "in", "Input Image");
					AddParameter(ParameterType_OutputFilename, "out", "Ouput Label Image");

					AddParameter(ParameterType_Choice, "criterion", "Homogeneity criterion to use");
					AddChoice("bs", "Baatz & Schape");
					AddChoice("ed", "Euclidean Distance");
					AddChoice("fls", "Full Lambda Schedule");

					AddParameter(ParameterType_Float, "threshold", "Threshold for the criterion");

					AddParameter(ParameterType_Int, "niter", "Number of iterations");
					SetDefaultParameterFloat("niter", 0);
					MandatoryOff("niter");

					AddParameter(ParameterType_Int, "speed", "Activate it to boost the segmentation speed");
					SetDefaultParameterFloat("speed", 0);
					MandatoryOff("speed");

					// For Baatz & Schape
					AddParameter(ParameterType_Float, "cw", "Weight for the spectral homogeneity");
					SetDefaultParameterFloat("cw", 0.5);
					MandatoryOff("cw");
					AddParameter(ParameterType_Float, "sw", "Weight for the spatial homogeneity");
					SetDefaultParameterFloat("sw", 0.5);
					MandatoryOff("sw");
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

					// Output label file name.
					const std::string labelImageFileName = GetParameterString("out");

					// Criterion selected
					const std::string selectedCriterion = GetParameterString("criterion");

					// Threshold
					float threshold = GetParameterFloat("threshold");

					const unsigned int niter = GetParameterInt("niter");
					const int speed = GetParameterInt("speed");

					typedef otb::ImageFileWriter<LabelImageType> LabelImageWriter;

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


						LabelImageWriter::Pointer labelWriter = LabelImageWriter::New();
						labelWriter->SetFileName(labelImageFileName);
						labelWriter->SetInput(segmenter.GetLabeledClusteredOutput());
						labelWriter->Update();
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

						LabelImageWriter::Pointer labelWriter = LabelImageWriter::New();
						labelWriter->SetFileName(labelImageFileName);
						labelWriter->SetInput(segmenter.GetLabeledClusteredOutput());
						labelWriter->Update();
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

						LabelImageWriter::Pointer labelWriter = LabelImageWriter::New();
						labelWriter->SetFileName(labelImageFileName);
						labelWriter->SetInput(segmenter.GetLabeledClusteredOutput());
						labelWriter->Update();
					}
				}
		};
	} // end of namespace Wrapper
	
} // end of namespace otb

