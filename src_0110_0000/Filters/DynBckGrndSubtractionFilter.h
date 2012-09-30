
#pragma once 

#include "ImageAbstractFilter.h"

class DynBckGrndSubtractionFilter : public ImageAbstractFilter
{
	int type; 
	BackgroundSubtractorMOG backgroundSubtractorMOG;
	BackgroundSubtractorMOG2 backgroundSubtractorMOG2;

public:
	DynBckGrndSubtractionFilter(int histLength=10, int nmixtures = 3, double bckRatio = 0.7, double sigma = 1.):
	  backgroundSubtractorMOG(histLength, nmixtures, bckRatio, sigma)
	{type = 0;}

	DynBckGrndSubtractionFilter(int histLength, float varThres = 2.7, bool shw = false):
	  backgroundSubtractorMOG2(histLength, varThres, shw)
	{type = 1;}

	~DynBckGrndSubtractionFilter(){};

    inline Mat & ApplyFilter(Mat & image);
    inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);
};



inline Mat & DynBckGrndSubtractionFilter::ApplyFilter(Mat & inputImage)
{
	if (type == 0)
		backgroundSubtractorMOG(inputImage, inputImage);
	else 
		backgroundSubtractorMOG2(inputImage, inputImage);

	return inputImage;
}

inline void DynBckGrndSubtractionFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	buffer->setOutputImages(ApplyFilter(buffer->getOutputImage(input.getChannelNumber())), input.getChannelNumber());
}
