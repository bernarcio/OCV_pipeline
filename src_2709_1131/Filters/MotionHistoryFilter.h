#pragma once

#include "ImageAbstractFilter.h"

#include <time.h>

/*
	Problem with SONY camera.
	mhi stores everything and mask contains all info from the very begining
*/


class MotionHistoryFilter : public ImageAbstractFilter
{
	double mhiDuration;
	vector<Mat> buffer;
	int length;
	int counter;
	Mat mhi;

	int outputSize;

	int diffThreshold; // parameter in threshold, e.g = 30

public:
	
	MotionHistoryFilter(int n=4, double duration=1.0, int arg=30){
		mhiDuration = duration;
		length = n;
		counter = 0;
		// reserve space in the buffer
		for (int i=0;i<length;i++)
			buffer.push_back(Mat());

		diffThreshold = arg;
	}
	~MotionHistoryFilter(){};

	
    inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};



inline void MotionHistoryFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * pipelineBuffer)
{
	Mat inputImage = pipelineBuffer->getOutputImage(input.getChannelNumber());
	
	if (mhi.data == NULL){ // first call
		mhi = Mat(inputImage.rows, inputImage.cols, CV_32F, Scalar(0.));		
	}
	if (buffer[0].data == NULL){
		vector<Mat>::iterator iter = buffer.begin();
		for (;iter!=buffer.end();++iter)
			(*iter) = Mat(inputImage.rows, inputImage.cols, CV_8U, Scalar(0));
	}

	// convert inputImage into gray level image:
	cvtColor(inputImage,buffer[counter], CV_BGR2GRAY, 0);

	double timestamp = (double) clock()/CLOCKS_PER_SEC; // get current time in seconds
		
	int ind = counter;
	counter = (counter + 1) % length;
	Mat silh = buffer[counter];
	absdiff(buffer[ind], silh, silh);
			

	threshold(silh, silh, diffThreshold, 1, CV_THRESH_BINARY );
	updateMotionHistory(silh, mhi, timestamp, mhiDuration);
	
	Mat mask;
	mhi.convertTo(mask,CV_8U, 255./mhiDuration, (mhiDuration - timestamp)*255./mhiDuration);
	

	//Mat channels[3] = {mask, 0*mask, 0*mask}; 
	//inputImage *= 0;
	//merge(channels, 3, inputImage);
	
	// outputs: Mat mask, 
	//pipelineBuffer->setOutputImages(inputImage, input.getChannelNumber());
	pipelineBuffer->setOutputImages(mask, input.getChannelNumber());
	//pipelineBuffer->setOutputImages(mhi, input.getChannelNumber());
	// set into internal memory: Mat mhi, double timestamp
	pipelineBuffer->setInternalImage("mhi", mhi);	
	InternalVariable v(timestamp);
	pipelineBuffer->setInternalVariable("timestamp", v);
	InternalVariable d(mhiDuration);
	pipelineBuffer->setInternalVariable("mhiDuration", d);
	
	
}
