#ifndef DRAWHISTOGRAM_H
#define DRAWHISTOGRAM_H

#include "ImageAbstractFilter.h"


class DrawHistogram : public ImageAbstractFilter
{

public:
	enum ImageType {GRAY, RGB};
	DrawHistogram(ImageType type=GRAY, string name="Histogram"){
		this->type = type;	
		histWindowName = name;
	};
	~DrawHistogram(){};


	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

protected:
	inline void drawHist(Mat & hist, int sz, string name="hist");

private:
	ImageType type;
	string histWindowName;
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
		histSize[0] = 256;
		channels[0] = 0;
		range[0] = 0; range[1] = 255;
	}	
	const float * ranges[] = {range};
	
	calcHist(&img,1,channels,Mat(),hist,1,histSize,ranges);
	
	drawHist(hist, 256, histWindowName);

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