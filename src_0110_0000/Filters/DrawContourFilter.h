
#ifndef DRAWCONTOURFILTER_H
#define DRAWCONTOURFILTER_H


#include "ImageAbstractFilter.h"


class DrawContourFilter : public ImageAbstractFilter
{

public:
	DrawContourFilter(){
	};
	~DrawContourFilter(){};

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};


inline void DrawContourFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer){

	Mat img = buffer->getOutputImage(input.getChannelNumber());
	if (img.channels() == 3)
		cvtColor(img,img,CV_BGR2GRAY);
		

	vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

	findContours(img, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
		
    if( contours.size() == 0 )
        return;
        
    // iterate through all the top-level contours,
    // draw each connected component with its own random color
	Mat output = Mat::zeros(img.rows, img.cols, CV_8UC3);
    int idx = 0;
	for( ; idx >= 0; idx = hierarchy[idx][0] )
	{
		Scalar color( rand()&255, rand()&255, rand()&255 );
		drawContours( output, contours, idx, color, CV_FILLED, 8, hierarchy );
	}

	// return :
	buffer->setOutputImages(output,input.getChannelNumber());

}


#endif