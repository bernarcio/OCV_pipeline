#pragma once

#include "ImageAbstractFilter.h"

class MedianFilter : public ImageAbstractFilter
{
	
	int size;

public:
	MedianFilter(int s) {size=s;};
	~MedianFilter(){};
	inline Mat & ApplyFilter(Mat & image);
	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};



inline Mat & MedianFilter::ApplyFilter(Mat & inputImage)
{
	
	medianBlur(inputImage,inputImage,size);
	
	return inputImage;
}


inline void MedianFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	buffer->setOutputImages(ApplyFilter(buffer->getOutputImage(input.getChannelNumber())), input.getChannelNumber());
}