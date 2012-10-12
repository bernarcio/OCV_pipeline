#pragma once

#include "ImageAbstractFilter.h"

class BlurFilter : public ImageAbstractFilter
{
	
	

public:
	enum BlurType {SIMPLE_BLUR, GAUSSIAN_BLUR};
	BlurFilter(BlurType type, int w=3, int h=3) {
		size = Size(w,h);
		this->type=type;

		this->sigmaX = 1.0;
		this->sigmaY = 1.0;
	};
	BlurFilter(BlurType type=GAUSSIAN_BLUR, int w=3, int h=3, double sigmaX=1.0, double sigmaY=1.0){
		size = Size(w,h);
		this->type=type;
		this->sigmaX = sigmaX;
		this->sigmaY = sigmaY;
	}

	~BlurFilter(){};
    inline Mat & ApplyFilter(Mat & image);
    inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

private:
	Size size;
	double sigmaX;
	double sigmaY;
	BlurType type;

};



inline Mat & BlurFilter::ApplyFilter(Mat & inputImage)
{
	if (type == SIMPLE_BLUR)
		blur(inputImage, inputImage,size);
	else if (type == GAUSSIAN_BLUR)
		GaussianBlur(inputImage, inputImage, size, sigmaX, sigmaX);

	return inputImage;
}

inline void BlurFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	buffer->setOutputImages(ApplyFilter(buffer->getOutputImage(input.getChannelNumber())), input.getChannelNumber());
}
