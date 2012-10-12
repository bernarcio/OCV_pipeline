#pragma once

#include "ImageAbstractFilter.h"

class RescaleFilter : public ImageAbstractFilter
{
	
	double factor; 
	int type; // if type = 1 -> rescale back to 255 after color simplifications
	
public:
	RescaleFilter(double fact, int t = 1) {factor=fact; type = t;};
	~RescaleFilter(){};
	inline Mat & ApplyFilter(Mat & image);
	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};



inline Mat & RescaleFilter::ApplyFilter(Mat & inputImage)
{
	
	inputImage = inputImage*factor;
	if (type == 1)
		inputImage = inputImage*(255.0/(factor*255.0));
	
	return inputImage;
}

inline void RescaleFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	buffer->setOutputImages(ApplyFilter(buffer->getOutputImage(input.getChannelNumber())), input.getChannelNumber());
}