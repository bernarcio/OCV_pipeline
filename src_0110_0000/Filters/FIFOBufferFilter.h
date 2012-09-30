#ifndef FIFOBUFFERFILTER_H
#define FIFOBUFFERFILTER_H

#include "ImageAbstractFilter.h"

class FIFOBufferFilter : public ImageAbstractFilter
{

public:
	enum OperationType {SUM};
	FIFOBufferFilter(int N=4, OperationType t=SUM){
		size = N;
		type = t;
		counter = 0;
		
	}

	~FIFOBufferFilter(){}
	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

protected:
	OperationType type; 
	int size;
	vector<Mat> fifo;
	Mat outputImage;
	int counter;
};


inline void FIFOBufferFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{	
	Mat inputImage = buffer->getOutputImage(input.getChannelNumber());
	Mat tempImg;
	if (fifo.size()<size){
		
		fifo.push_back(inputImage);
		tempImg = Mat(inputImage.rows, inputImage.cols, inputImage.type(),Scalar(0));
		
	}else{
		tempImg = fifo[counter % size];
		fifo[counter % size] = inputImage;
	}

	if (outputImage.data == NULL){
			//outputImage = Mat(inputImage.rows, inputImage.cols, inputImage.type());
			outputImage = fifo[counter]; // counter = 0;
	}
	else{
		if (type == SUM){
			outputImage -= (1./size)*tempImg;
			outputImage = (min(counter, size)*1.0/min(counter + 1, size))*(outputImage) + (1./min(counter + 1, size))*fifo[counter % size];
		}
	}

	counter++; 

	// return:
	buffer->setOutputImages(outputImage, input.getChannelNumber());

}



#endif
