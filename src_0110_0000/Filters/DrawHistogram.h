#ifndef DRAWHISTOGRAM_H
#define DRAWHISTOGRAM_H

#include "ImageAbstractFilter.h"

/*
	Filter to draw the histogram of the image
	GRAY - one channel - histogram is implemented
		int startBoundary - if user wants to start the histogram from some other value than 0 -> neglect some dark colors (when mask is applied)

*/


class DrawHistogram : public ImageAbstractFilter
{

public:
	enum ImageType {GRAY, RGB};
	DrawHistogram(ImageType type=GRAY, string name="Histogram", int startBoundary=0){
		this->type = type;	
		histWindowName = name;

		this->startBoundary=startBoundary; 


	};
	~DrawHistogram(){};


	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

protected:
	inline void drawHist(Mat & hist, int sz, string name="hist");

private:
	ImageType type;
	string histWindowName;
	int startBoundary;
};




inline void DrawHistogram::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{

	Mat img = buffer->getOutputImage(input.getChannelNumber());	
	if (img.channels()==3){
		if (type == GRAY){
			cvtColor(img,img,CV_BGR2GRAY); // ok, but very bright background regions spoils the segmentation
		}
	}

	// Compute the histogram of img:
	Mat hist;
	int histSize[] = {256}; 
	int channels[] = {0};
	float range[] = {0,255}; 
	
	if (type == GRAY){
		channels[0] = 0;
		if (startBoundary == 0){
			range[0] = 0; range[1] = 255;
			histSize[0] = range[1] - range[0] + 1;
		}else{
			range[0] = startBoundary; range[1] = 255;
			histSize[0] = range[1] - range[0] + 1;
		}
	}	
	const float * ranges[] = {range};
	
	calcHist(&img,1,channels,Mat(),hist,1,histSize,ranges);
	drawHist(hist, 256 - startBoundary, histWindowName);

}


inline void DrawHistogram::drawHist(Mat & hist, int sz, string name)
{

	// show histogram:
	double maxVal=0;
	minMaxLoc(hist, 0, &maxVal,0,0);
	Mat histImg = Mat(150,sz,CV_32F,Scalar(0));

	for (int i=0;i<sz;i++){
	
		float binVal = hist.at<float>(i);
	    binVal = floor(100*binVal/maxVal);
		int index = 110 - (int) binVal;
		histImg.at<float>(index, i) = 255;
		
		// draw 2 horizontal lines:
		//histImg.at<float>(10, i) = 50;
		histImg.at<float>(110, i) = 255;

	}
	
	imshow(name, histImg);
}



#endif