#ifndef HISTMOTIONDETECTIONFILTER_H
#define HISTMOTIONDETECTIONFILTER_H

#include "ImageAbstractFilter.h"

class HistMotionDetectionFilter : public ImageAbstractFilter
{
	
	Mat meanImage;
	Mat meanImage2;
	double alpha;

	inline void drawHist(Mat & hist, int sz, string name="hist");

public:
	HistMotionDetectionFilter(double a=0.007){
		alpha = a;
	};

	~HistMotionDetectionFilter(){};

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};




inline void HistMotionDetectionFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	
	Mat img = buffer->getOutputImage(input.getChannelNumber());	
	if (img.channels()==3){
		// RGB to GRAY:
		cvtColor(img,img,CV_BGR2GRAY); // ok, but very bright background regions spoils the segmentation

		// RGB => H or S or V
		//cvtColor(img,img,CV_BGR2HSV);
		//Mat hsv[3];
		//split(img, hsv);
		//img = hsv[0]; // more or less admissible, very noisy
		//img = hsv[1]; // not bad, noisy
		//img = hsv[2]; // as gray
		//img = ( hsv[0] & hsv[1] ) | hsv[2]; // noisy

	}
	
	//equalizeHist(img,img);

	if (meanImage.data == NULL){
		img.convertTo(meanImage, CV_32F);
		
		if (meanImage2.data == NULL)
			img.convertTo(meanImage2, CV_32F);

		return;
	}



	// accumulate: meanImage = (1-alpha)*meanImage + alpha*img
	accumulateWeighted(img,meanImage,alpha,Mat());
	accumulateWeighted(img,meanImage2,50*alpha,Mat());
	//blur(meanImage,meanImage,Size(3,3));



	Mat meanImage8U, meanImage2_8U;
	meanImage.convertTo(meanImage8U, CV_8U);
	meanImage2.convertTo(meanImage2_8U, CV_8U);
	
	imshow("meanImage",meanImage8U);
	imshow("meanImage2",meanImage2_8U);

	Mat diffImg;
	absdiff(meanImage8U,meanImage2_8U,diffImg);
	//diffImg = meanImage8U & meanImage2_8U;
	threshold(diffImg,diffImg,20, 255,CV_THRESH_BINARY);
	imshow("diffImage", diffImg);
	
	//return :
	buffer->setOutputImages(diffImg, input.getChannelNumber());

	// Compute the histogram of meanImage:
	//Mat meanHist;
	//int histSize[] = {256}; 
	//int channels[] = {0};
	//float range[] = {0,255}; const float * ranges[] = {range};
	//calcHist(&meanImage,1,channels,Mat(),meanHist,1,histSize,ranges);

	//// Compute 
	//Mat currHist;
	//calcHist(&img,1,channels,Mat(),currHist,1,histSize,ranges);
	//
	//// show mean histogram:
	//drawHist(meanHist,256,"mean hist");
	//drawHist(currHist,256,"curr hist");

	//Mat diffHist;
	//double maxVal=0; minMaxLoc(currHist, 0, &maxVal,0,0);
	//absdiff(currHist, meanHist, diffHist);
	////threshold(diffHist,diffHist,maxVal*0.15,0.9,CV_THRESH_BINARY);
	//drawHist(diffHist, 256, "diff hist");


}


inline void HistMotionDetectionFilter::drawHist(Mat & hist, int sz, string name)
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