#ifndef GENERICDESCRIPTOREXTRACTORFILTER_H
#define GENERICDESCRIPTOREXTRACTORFILTER_H

#include "ImageAbstractFilter.h"

class GenericDescriptorExtractorFilter : public ImageAbstractFilter
{

	string extractorType;
	Ptr<DescriptorExtractor> descriptorExtractor;
	string nameOfDescriptorsMatrix;
	string nameOfKeyPointsVariable;

public:

	enum ExtractorType {SIFT, SURF, ORB, BRIEF};
	enum AdditionalType {NO, OPPONENT};

	GenericDescriptorExtractorFilter(ExtractorType type, AdditionalType at=NO, string nameOfKeyPointsVariable="gdf_keypoints",string nameOfDescriptorsMatrix="gdef_descriptors"){
		
		this->nameOfDescriptorsMatrix = nameOfDescriptorsMatrix;
		this->nameOfKeyPointsVariable = nameOfKeyPointsVariable;

		if (type == SIFT){
			extractorType = "SIFT";
		}else if (type == SURF){
			extractorType = "SURF";
		}else if (type == ORB){
			extractorType = "ORB";
		}else if (type == BRIEF){
			extractorType = "BRIEF";
		}

		if (at == OPPONENT){
			extractorType = "Opponent"+extractorType;
		}

		descriptorExtractor = DescriptorExtractor::create(extractorType);
	
	};
	
	~GenericDescriptorExtractorFilter(){};

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};




inline void GenericDescriptorExtractorFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	Mat img = buffer->getOutputImage(input.getChannelNumber());
	//if(img.channels()==3)
	//	cvtColor(img,img,CV_BGR2GRAY);
	

	// read keypoints:
	vector<KeyPoint> keypoints, *kp = buffer->getInternalKeyPoints(nameOfKeyPointsVariable);
	if (kp!=NULL){
			keypoints = *kp;
	}else{
		cerr << "GenericDescriptorExtractorFilter: Keypoints was not found at name : " << nameOfKeyPointsVariable << endl;
		return;
	}

	Mat descriptors;

	// compute descriptors:
	descriptorExtractor->compute(img,keypoints,descriptors);

	// store in the buffer:
	buffer->setInternalImage(nameOfDescriptorsMatrix, descriptors);


}


#endif