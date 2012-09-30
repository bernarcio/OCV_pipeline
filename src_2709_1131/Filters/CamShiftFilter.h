#pragma once

#include "ImageAbstractFilter.h"

class CamShiftFilter : public ImageAbstractFilter
{
	
	int iterations;

public:
	CamShiftFilter(int iter=10) {iterations=iter;};
	~CamShiftFilter(){};
	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};



inline void CamShiftFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * pipelineBuffer)
{
	Mat inputImage = pipelineBuffer->getOutputImage(input.getChannelNumber());
	
	//cvtColor(inputImage,inputImage,CV_BGR2GRAY);

	Rect trackWindow = Rect(0,0,inputImage.cols, inputImage.rows);

	try
	{
		Mat probaImage = (1./255.)*inputImage;
		RotatedRect box = CamShift(probaImage, trackWindow, TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, iterations, 1 ));
	
		Rect rect = box.boundingRect(); 
		pipelineBuffer->setOutputImages(pipelineBuffer->getOutputImage("orig"), input.getChannelNumber());
		rectangle(pipelineBuffer->getOutputImage(input.getChannelNumber()), rect,Scalar(255,0,0),1);
		
	}
	catch( cv::Exception & excp){
		// no tracking objects				
		cerr << "exception is drawn using camshift" << endl;
    }


}