
#ifndef CALCOPTICALFLOWFILTER_H
#define CALCOPTICALFLOWFILTER_H

#include "ImageAbstractFilter.h"

#include <cv.h>
#include <cxcore.h>

const int MAX_CORNERS = 500;

class CalcOpticalFlowFilter : public ImageAbstractFilter
{

	// by default we use cvGoodFeaturesToTrack() to track points

	bool internalDetector;
	string nameOfKeyPointsVariable;
	int win_size;

	vector<Mat> buffer;
	int bufferSize;
	int counter;

public:
	enum OpticalFlowType {LUCAS_KANADE};
	CalcOpticalFlowFilter(OpticalFlowType type=LUCAS_KANADE, bool internaldetector=true, string var_keypoints=""){
		internalDetector = internaldetector;
		if (!internalDetector)
			nameOfKeyPointsVariable = var_keypoints;

		win_size=10;
		counter = 0;
		bufferSize = 5;

	};

	~CalcOpticalFlowFilter(){};

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);


};




inline void CalcOpticalFlowFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * pipelineBuffer)
{

	Mat img = pipelineBuffer->getOutputImage(input.getChannelNumber());
	if (img.channels()==3)
			cvtColor(img,img,CV_BGR2GRAY);

	if (buffer.size()<bufferSize){
		buffer.push_back(img);
		counter++;
		return;
	}else{
		buffer[counter % bufferSize] = img;
		counter++;
	}

	int ind2 = (counter) % bufferSize;
	int ind1 = (counter + 1) % bufferSize;
				
	int corner_count = MAX_CORNERS;
	CvPoint2D32f * cornersA = new CvPoint2D32f[MAX_CORNERS];

	//imshow("prev", buffer[ind1]);
	//imshow("now", buffer[ind2]);

	IplImage * imgA = &(buffer[ind1]).operator IplImage();
	IplImage * imgB = &(buffer[ind2]).operator IplImage();
	
	if (internalDetector){
		

		IplImage * eig_image = cvCreateImage(cvSize(img.cols,img.rows), IPL_DEPTH_32F, 1);
		IplImage * tmp_image = cvCreateImage(cvSize(img.cols,img.rows), IPL_DEPTH_32F, 1);
	

		cvGoodFeaturesToTrack(
			imgA,
			eig_image,
			tmp_image,
			cornersA,
			&corner_count,
			0.01,
			5.0,
			0,
			3,
			0,
			0.04
			);

		cvFindCornerSubPix(
			imgA,
			cornersA,
			corner_count,
			cvSize(win_size, win_size),
			cvSize(-1,-1),
			cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03)
			);

	}else{
		// read the pipelineBuffer :
		vector<KeyPoint> keyPoints, *kp = pipelineBuffer->getInternalKeyPoints(nameOfKeyPointsVariable);		
		if (kp!=NULL){
			keyPoints = *kp;
		}else{
			cerr << "CalcOpticalFlowFilter: Keypoints was not found at name : " << nameOfKeyPointsVariable << endl;
			return;
		}


		int mc = (corner_count < keyPoints.size()) ? corner_count : keyPoints.size();
		for (int i=0;i<mc;i++)
			cornersA[i] = cvPoint2D32f(keyPoints[i].pt.x, keyPoints[i].pt.y);
		for (int i=mc;i<corner_count;i++)
			cornersA[i] = cvPoint2D32f(0.,0.);
	}


	// call the Lucas-Kanade algorithm:
	char features_found[MAX_CORNERS];
	float feature_errors[MAX_CORNERS];

	CvSize pyr_sz = cvSize(imgA->width+8, imgB->height/3);

	IplImage * pyrA = cvCreateImage(pyr_sz, IPL_DEPTH_32F, 1);
	IplImage * pyrB = cvCreateImage(pyr_sz, IPL_DEPTH_32F, 1);

	CvPoint2D32f * cornersB = new CvPoint2D32f[MAX_CORNERS];

	cvCalcOpticalFlowPyrLK(
		imgA,
		imgB,
		pyrA,
		pyrB,
		cornersA,
		cornersB,
		corner_count,
		cvSize(win_size, win_size),
		5,
		features_found,
		feature_errors,
		cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 ),
		0
		);

	// draw motion vectors:
	Mat out = pipelineBuffer->getOutputImage("orig");
	for (int i=0;i<corner_count;i++){
		
		if(features_found[i]==0 || feature_errors[i]>550)
			continue;

		Point p1(cvRound(cornersA[i].x), cvRound(cornersA[i].y));
		Point p2(cvRound(cornersB[i].x), cvRound(cornersB[i].y));
	
		line(out, p1, p2, Scalar(0,0,255));
	}
	
	// return:
	pipelineBuffer->setOutputImages(out,input.getChannelNumber());

}

#endif