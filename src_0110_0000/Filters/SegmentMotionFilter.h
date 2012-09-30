#pragma once 


#include "ImageAbstractFilter.h"
#include <time.h>

class SegmentMotionFilter : public ImageAbstractFilter
{
	
	Mat segmask;
	vector<Rect> boundingRects;
	double MAX_TIME_DELTA;
	int boxSizeLimit;

public:
	SegmentMotionFilter(){
		MAX_TIME_DELTA = 0.5;
		boxSizeLimit=30;
	}
	~SegmentMotionFilter(){}

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);
};



inline void SegmentMotionFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	
	Mat inputImage = buffer->getOutputImage(input.getChannelNumber());

	if (segmask.data == NULL)
		segmask = Mat(inputImage.rows, inputImage.cols, CV_32F);
	
	double timestamp = buffer->getInternalVariable("timestamp").GetDoubleValue();
	double mhiDuration = buffer->getInternalVariable("mhiDuration").GetDoubleValue();
	// Segment motion:
	//Mat mhi = buffer->getInternalImage("mhi");
	//segmentMotion(mhi, segmask, boundingRects, timestamp, MAX_TIME_DELTA);

	segmentMotion(inputImage, segmask, boundingRects, timestamp, MAX_TIME_DELTA);
	

	Mat mask = inputImage * 255./mhiDuration + (mhiDuration - timestamp)*255./mhiDuration;
	buffer->setOutputImages(mask, input.getChannelNumber());

	// iterate through the motion components
	vector<Rect>::iterator iter = boundingRects.begin();
	for (; iter!=boundingRects.end();++iter){
		
		// neglect small rectangles:
		if ((*iter).height + (*iter).width < boxSizeLimit)
			continue;

		// neglect to huge rectangles:
		if ((*iter).height + (*iter).width > 0.75*(inputImage.rows + inputImage.cols))
			continue;

		// neglect rectangles : check for the case of little motion
		// calculate number of points within silhouette ROI
		//double count = norm(silh((*iter)),CV_L1);
		//if (count < (*iter).height * (*iter).width * 0.05) // if number of points is less than 5%
		//		continue;
		
		// draw the rectangle:
		rectangle(buffer->getOutputImage("orig"), *iter, Scalar(0,0,255));
	
	
	}



}