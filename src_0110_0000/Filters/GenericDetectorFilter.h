#ifndef GENERICDETECTORFILTER_H
#define GENERICDETECTORFILTER_H


#include "ImageAbstractFilter.h"

class GenericDetectorFilter : public ImageAbstractFilter
{

	string detectorType;
	Ptr<FeatureDetector> featureDetector;
	string nameOfKeyPointsVariable;

public:
	enum DetectorType {FAST, STAR, SIFT, SURF, ORB, MSER, GoodFeatToTrk, HARRIS, DENSE, SimpleBlob};
	enum AdditionalType {NO, GRID, PYRAMID};
	GenericDetectorFilter(DetectorType type, AdditionalType at=NO, string _nameOfKeyPointsVariable="gdf_keypoints"){
	
		nameOfKeyPointsVariable = _nameOfKeyPointsVariable;

		if (type == FAST){
			detectorType = "FAST";
		}else if (type == STAR){
			detectorType = "STAR";
		}else if (type == SIFT){
			detectorType = "SIFT";
		}else if (type == SURF){
			detectorType = "SURF";
		}else if (type == ORB){
			detectorType = "ORB";
		}else if (type == MSER){
			detectorType = "MSER";
		}else if (type == GoodFeatToTrk){
			detectorType = "GFTT";
		}else if (type == HARRIS){
			detectorType = "HARRIS";
		}else if (type == DENSE){
			detectorType = "Dense";
		}else if (type == SimpleBlob){
			detectorType = "SimpleBlob";
		}

		if (at == GRID){
			detectorType = "Grid"+detectorType;
		}else if (at == PYRAMID){
			detectorType = "Pyramid"+detectorType;
		}

		featureDetector = FeatureDetector::create(detectorType);

		


	};
	~GenericDetectorFilter(){};

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};



inline void GenericDetectorFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	Mat img = buffer->getOutputImage(input.getChannelNumber());

	//if(img.channels()==3)
	//	cvtColor(img,img,CV_BGR2GRAY);

	vector<KeyPoint> keyPoints;
	
	featureDetector->detect(img,keyPoints, Mat());


	Mat origImg = buffer->getOutputImage("orig");
	// draw keypoints:
	if (img.rows != origImg.rows || img.cols != origImg.cols){
				
		//if (img.channels() ==1 )
		//	cvtColor(img,img,CV_GRAY2BGR);
		
		
		drawKeypoints(img, keyPoints, img, Scalar(255,0,0));		

	}else{
		drawKeypoints(origImg, keyPoints,origImg, Scalar(255,0,0));
	}			
	
	
	// return :
	buffer->setInternalKeyPoints(nameOfKeyPointsVariable,keyPoints);
	
}

#endif