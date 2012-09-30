#ifndef FOURIERTRANSFORMFILTER_H
#define FOURIERTRANSFORMFILTER_H

#include "ImageAbstractFilter.h"


class FourierTransformFilter : public ImageAbstractFilter
{

public:
	FourierTransformFilter(bool inv=false, bool realoutput=true){
		inverse = inv;
		realOutput = realoutput;
	}
	~FourierTransformFilter(){}
	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

protected:
	bool inverse;
	bool realOutput;
	
};


inline void FourierTransformFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	Mat inputImage = buffer->getOutputImage(input.getChannelNumber());
	Mat img;

	if (inputImage.channels() == 3)
		cvtColor(inputImage, img, CV_BGR2GRAY);

	img.convertTo(img,CV_32F);

	// setup flags:
	int flags=0;
	if (realOutput)
		flags = DFT_REAL_OUTPUT;
	else 
		flags = DFT_COMPLEX_OUTPUT;

	if (inverse)
		flags |= DFT_INVERSE;
	
	
	// transform:
	dft(img,img, flags);

	// if complex take the absolute value:
	if (!realOutput){
		Mat ch[2];
		split(img, ch);
		magnitude(ch[0], ch[1], img);
	}

	// return 
	// convert to 8U:
	img.convertTo(img,CV_8U,255./sqrt((double) (img.rows*img.cols)));
	buffer->setOutputImages(img, input.getChannelNumber());

}

#endif