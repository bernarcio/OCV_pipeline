#pragma once

#include "ImageAbstractFilter.h"

class ContourFilter : public ImageAbstractFilter
{
	
	int size; 
	int type; // type = 0 - Sobel , type = 1 - Canny
	double threshold_1;
	double threshold_2;


public:
	ContourFilter(int s=3) {size=s; type=0;}; // Sobel
	ContourFilter(int s, double t1=30, double t2=150) {size=s; type=1; threshold_1 = t1;threshold_2 = t2;}; // Canny
	~ContourFilter(){};
	inline Mat & ApplyFilter(Mat & image);
	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);
};



inline Mat & ContourFilter::ApplyFilter(Mat & inputImage)
{
	cvtColor(inputImage,inputImage, CV_BGR2GRAY, 0);
	if (type==0){
		Mat img_x, img_y;
		Sobel(inputImage, img_x, CV_32F, 1, 0, size, 1,0, 4);
		Sobel(inputImage, img_y, CV_32F, 0, 1, size, 1,0, 4);
		pow(img_x,2,img_x);
		pow(img_y,2,img_y);
		sqrt(img_x + img_y, inputImage);
	}		
	else if (type==1){
		
		Canny(inputImage,inputImage,threshold_1,threshold_2,size);
	}

	return inputImage;
}


inline void ContourFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	buffer->setOutputImages(ApplyFilter(buffer->getOutputImage(input.getChannelNumber())), input.getChannelNumber());
}