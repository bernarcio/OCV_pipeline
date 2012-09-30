#pragma once

#include "ImageAbstractFilter.h"

class BlurFilter : public ImageAbstractFilter
{
	
	Size size;

public:
	BlurFilter(int w=3, int h=3) {size = Size(w,h);};
	~BlurFilter(){};
    inline Mat & ApplyFilter(Mat & image);
    inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};



inline Mat & BlurFilter::ApplyFilter(Mat & inputImage)
{
	blur(inputImage, inputImage,size);
	return inputImage;
}

inline void BlurFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	buffer->setOutputImages(ApplyFilter(buffer->getOutputImage(input.getChannelNumber())), input.getChannelNumber());
}
