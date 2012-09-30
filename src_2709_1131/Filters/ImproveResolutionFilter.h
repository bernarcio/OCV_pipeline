#pragma once

#include "ImageAbstractFilter.h"


class ImproveResolutionFilter : public ImageAbstractFilter
{
	int numberOfFrames; // need to be pair
	int counter;
	Mat newResolutionImage;

public:
	ImproveResolutionFilter(int nf=4){
		(nf % 2 == 0) ? numberOfFrames=nf : numberOfFrames=nf-1;
		counter = 0;
	}
	~ImproveResolutionFilter(){};

	Mat & ApplyFilter(Mat & image);
	void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);
	
	void InsertImage(Mat & FinalImage, Mat & imageToInsert, int linearposition);


};



Mat & ImproveResolutionFilter::ApplyFilter(Mat & inputImage)
{
	if (newResolutionImage.data == NULL)
		newResolutionImage = Mat(inputImage.rows * numberOfFrames/2, inputImage.cols * numberOfFrames/2, inputImage.type(),Scalar(0));

	InsertImage(newResolutionImage, inputImage, counter);
	counter = ( counter + 1 ) % numberOfFrames;
	inputImage = newResolutionImage;
	return inputImage;
}


void ImproveResolutionFilter::InsertImage(Mat & FinalImage, Mat & imageToInsert, int linearPosition)
{
	
	int n = numberOfFrames/2;
	int b = (linearPosition) % n, a = (int) ((linearPosition)/n);  // 0 % 2 = 0, 1 % 2 = 1; 2 % 2 = 0 etc 
	
	for (int i=0;i<imageToInsert.rows;i++){
		for (int j=0;j<imageToInsert.cols;j++){
			newResolutionImage.at<Vec3b>(n*i + a, n*j + b) = imageToInsert.at<Vec3b>(i,j);		
		}
	}

}

inline void ImproveResolutionFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	buffer->setOutputImages(ApplyFilter(buffer->getOutputImage(input.getChannelNumber())), input.getChannelNumber());
}
