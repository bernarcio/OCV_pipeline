#pragma once

#include "ImageAbstractFilter.h"

class MorphoFilter : public ImageAbstractFilter
{
	int shape;
	int type;
	int size;
	int numberOfIterations;

public:
	MorphoFilter(int s=3, int sh = MORPH_ELLIPSE, int t = MORPH_CLOSE, int niter=1) {
		size = s; 
		shape = sh; 
		type = t;
		numberOfIterations = niter;
	};
	~MorphoFilter(){};
    inline Mat & ApplyFilter(Mat & image);
    inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};



inline Mat & MorphoFilter::ApplyFilter(Mat & inputImage)
{
	
	
	// Add morpho.close
	Mat element = getStructuringElement( shape, Size( 2*size + 1, 2*size+1 ), Point( size, size ) );
	// Apply the specified morphology operation
	morphologyEx(inputImage,inputImage, type, element,Point(-1,-1),numberOfIterations);
		
	return inputImage;
}

inline void MorphoFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	buffer->setOutputImages(ApplyFilter(buffer->getOutputImage(input.getChannelNumber())), input.getChannelNumber());
}
