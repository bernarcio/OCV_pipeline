#ifndef HOUGHTRANSFORMATIONFILTER_H
#define HOUGHTRANSFORMATIONFILTER_H

#include "ImageAbstractFilter.h"


class HoughTransformationFilter : public ImageAbstractFilter
{

public:
	enum TransformationType {LINES, LINES_PROBA, CIRCLES};
	HoughTransformationFilter(TransformationType type=LINES_PROBA, bool useCannyForLines=true){
		this->type = type;	
		useCanny = useCannyForLines;
	};

	HoughTransformationFilter(TransformationType type=CIRCLES, bool useBlurForCircles=true, int blurSize=5, string nameOfCirclesCentersKeyPoints="hough_circles_centers_keypoints"){
		this->type = type;	
		useBlur = useBlurForCircles;
		this->blurSize = blurSize;
		this->nameOfCirclesCentersKeyPoints = nameOfCirclesCentersKeyPoints;
		
	};


	~HoughTransformationFilter(){};

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

private:
	TransformationType type;
	bool useBlur;
	int blurSize;
	bool useCanny;
	string nameOfCirclesCentersKeyPoints;
};




inline void HoughTransformationFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{

	Mat img = buffer->getOutputImage(input.getChannelNumber());	
	Mat gray;
	if (img.channels()==3){
			cvtColor(img,gray,CV_BGR2GRAY); 
	}else{
		gray = img;
	}

	
	
	if (type == CIRCLES){
		vector<Vec3f> output;
		// smooth otherwise a lot of false circles may be detected
		if (useBlur)
			GaussianBlur(gray, gray, Size(blurSize,blurSize),blurSize/6,blurSize/12); 

		// method = CV_HOUGH_GRADIENT
		// Inverse ratio of the accumulator resolution to the image resolution = 2
		// minDist = 50
		// param1 = 100
		// param2 = 100
		// minRadius = 20
		// maxRadius = image.cols
		HoughCircles(gray, output, CV_HOUGH_GRADIENT, 2, 50, 100,100,20,gray.cols);
	

		vector<KeyPoint> keypoints;
		for (int i=0; i<output.size();i++){
			// draw circles:
			Point center(cvRound(output[i][0]), cvRound(output[i][1]));
			int radius(cvRound(output[i][2]));

			Mat origImg = buffer->getOutputImage("orig");
			
			if (gray.rows != origImg.rows || gray.cols != origImg.cols){
				
				if (img.channels() ==1 )
					cvtColor(img,img,CV_GRAY2BGR);

				// draw the center: 
				circle(img, center, 3, Scalar(0,255,0), -1, 8, 0); 
				// draw the circle outline:
				circle(img, center, radius, Scalar(0,255,0), 3, 8, 0); 

			}else{
				// draw the center: 
				circle(origImg, center, 3, Scalar(0,255,0), -1, 8, 0); 
				// draw the circle outline:
				circle(origImg, center, radius, Scalar(0,255,0), 3, 8, 0); 
			}			

			// store in the buffer:
			KeyPoint kp((float) center.x, (float) center.y, (float) radius);
			keypoints.push_back(kp);
						
		}
		// return :
		buffer->setOutputImages(img, input.getChannelNumber());
		// store circles:
		buffer->setInternalKeyPoints(nameOfCirclesCentersKeyPoints, keypoints);


	}else if (type == LINES){
		vector<Vec2f> output;
		if (useCanny)
			Canny(gray, gray, 50, 200, 3 );

		HoughLines(gray, output, 1, CV_PI/180, 100);

		for (int i=0;i<output.size();i++){
			
			float rho = output[i][0];
			float theta = output[i][1];
			double a = cos(theta), b=sin(theta);
			double x0 = a*rho, y0 = b*rho;
			Point p1(cvRound(x0 + 1000*(-b)), cvRound(y0 + 1000*(a)));
			Point p2(cvRound(x0 - 1000*(-b)), cvRound(y0 - 1000*(a)));
			
			Mat origImg = buffer->getOutputImage("orig");
			line(origImg, p1, p2, Scalar(0,255,0), 3, 8);
		
		}
	
	}else if (type == LINES_PROBA){
	
		vector<Vec4i> output;
		if (useCanny)
			Canny(gray, gray, 50, 200, 3 );

		HoughLinesP(gray, output, 1, CV_PI/180, 80, 30, 10 );
		
		for (int i=0;i<output.size();i++){
			
			Point p1(output[i][0], output[i][1]);
			Point p2(output[i][2], output[i][3]);
			
			Mat origImg = buffer->getOutputImage("orig");
			line(origImg, p1, p2, Scalar(0,255,0), 3, 8 );
		
		}

	}
		



}


#endif