#ifndef GENERICDESCRIPTORMATCHERFILTER_H
#define GENERICDESCRIPTORMATCHERFILTER_H

#include "ImageAbstractFilter.h"

class GenericDescriptorMatcherFilter : public ImageAbstractFilter
{

public:
	enum MatcherType {BRUTEFORCE_L2, BRUTEFORCE_L1, BRUTEFORCE_HAMMING, BRUTEFORCE_HAMMING_LUT, FLANNED_BASED};
	enum MatchType {MATCH, KNN_MATCH, RADIUS_MATCH};

	GenericDescriptorMatcherFilter(MatcherType _type=BRUTEFORCE_L1, MatchType _mtype=MATCH, double max_distance=10., string nameOfDescriptorsMatrix="gdef_descriptors", string nameOfTrainDescriptorsMatrix ="learning_descriptor")
	{
		
		this->nameOfDescriptorsMatrix=nameOfDescriptorsMatrix;
		this->nameOfTrainDescriptorsMatrix=nameOfTrainDescriptorsMatrix;
		this->mtype = _mtype;
		knnValue = 0;
		radius = 0.0;

		if (_type == BRUTEFORCE_L2){
			type = "BruteForce";
		}else if (_type == BRUTEFORCE_L1){
			type = "BruteForce-L1";
		}else if (_type == BRUTEFORCE_HAMMING){
			type = "BruteForce-Hamming";
		}else if (_type == BRUTEFORCE_HAMMING_LUT){
			type = "BruteForce-HammingLUT";
		}else if (_type == FLANNED_BASED){
			type = "FlannBased";
		}

		isTrained = false;
		descriptorAdded = false;

		descriptorMatcher = DescriptorMatcher::create(type);

	};

	~GenericDescriptorMatcherFilter(){};

	inline void setKNNValue(int knnVal){ knnValue = knnVal;}
	inline void setRadius(double r) {radius = r;}


	// need to make sure that descriptors are of type CV_32F:
	//inline void addDescriptors(vector<Mat> & descriptors){
	//	descriptorMatcher->add(descriptors);
	//	descriptorAdded = true;
	//}

	inline void addDescriptors(PipelineBuffer * buffer){
		vector<Mat> descriptors; 
		Mat *img = buffer->getInternalImage(nameOfTrainDescriptorsMatrix);
		if (img != NULL){
			// type of trainDescriptor is CV_8U and not CV_32F, some assert from "match" function is not satisfied -> crash
			if ((*img).type()!=CV_32F){
				(*img).convertTo((*img), CV_32F);
			}
			descriptors.push_back(*(img));
		}else{
			cerr << "GenericDescriptorMatcherFilter: addDescriptors: Matrix is not found at name : " << nameOfTrainDescriptorsMatrix << endl;
			return;
		}

		//cerr << "training descriptors type = " << (*img).type() << " <-> CV_32F : " << CV_32F << " <-> CV_64F : " << CV_64F  << endl;
		
		descriptorMatcher->add(descriptors);
		descriptorAdded = true;
	}
	
	inline void addDescriptors(Mat & _descriptors){
		vector<Mat> descriptors; descriptors.push_back(_descriptors);
		descriptorMatcher->add(descriptors);
		descriptorAdded = true;
	}


	inline void trainMatcher(){
		if (descriptorAdded){
			descriptorMatcher->train();
			isTrained = true;
		}
	}

	inline void clearMatcher(){
		descriptorMatcher->clear();
        descriptorAdded = false;
        isTrained = false;
	}

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

	inline void FilterMatchesOnDistance(vector<DMatch> & m); // only for MATCH configuration



private:
	Ptr<DescriptorMatcher> descriptorMatcher;
	
	string type;
	MatchType mtype;
	string nameOfDescriptorsMatrix; 
	string nameOfTrainDescriptorsMatrix;

	int knnValue;
	double radius;

	double maxDistance; // distance between two matched descriptors
    
    bool isTrained;
    bool descriptorAdded;
	
};



inline void GenericDescriptorMatcherFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	Mat img2 = buffer->getOutputImage(input.getChannelNumber());
	
	if (!descriptorAdded){
        cout << " !!! No trained descriptors available. exit filter" << endl;
        return;
    }
    if (!isTrained)
		trainMatcher();

    vector<DMatch> matches;
    vector<vector<DMatch> > knnMatches;
	vector<vector<DMatch> > radiusMatches;
    // get query descriptors:
    Mat queryDescriptors, *qd = buffer->getInternalImage(nameOfDescriptorsMatrix);
	if (qd !=NULL ){
		queryDescriptors = *qd;		
	}else{
		cerr << "DescriptorMatcherFilter: Mat was not found at name : " << nameOfDescriptorsMatrix << endl;
		return;
	}
	
	// if empty -> go next step
	if (queryDescriptors.empty()){
		//cerr << "No query descriptors -> return" << endl;
		return;
	}

	// type of queryDescriptor is CV_8U and not CV_32F, some assert from "match" function is not satisfied -> crash
	if (queryDescriptors.type()!=CV_32F){
		queryDescriptors.convertTo(queryDescriptors, CV_32F);
	}
	//cerr << "query descriptors type = " << queryDescriptors.type() << " <-> CV_32F : " << CV_32F << " <-> CV_64F : " << CV_64F  << endl;
	
	// match:
	if (mtype == MATCH){
		
		descriptorMatcher->match(queryDescriptors, matches);
		
		//FilterMatchesOnDistance(matches);

		// store found matches:
		buffer->setInternalElement<vector<DMatch> >("matches", matches);

	}else if (mtype==KNN_MATCH){
		
		descriptorMatcher->knnMatch(queryDescriptors, knnMatches, knnValue);

		// filter

		// store found matches:
		buffer->setInternalElement<vector<vector<DMatch> > >("matches", knnMatches);
	
	}else if (mtype==RADIUS_MATCH){

		descriptorMatcher->radiusMatch(queryDescriptors, radiusMatches, radius);

		// store found matches:
		buffer->setInternalElement<vector<vector<DMatch> > >("matches", radiusMatches);
	}



}



inline void GenericDescriptorMatcherFilter::FilterMatchesOnDistance(vector<DMatch> & m)
{
	vector<DMatch>::iterator it = m.begin();
	for (;it!=m.end();){
		if (it->distance > maxDistance){
			it = m.erase(it);
		}else{
			it++;
		}	
	}
}



#endif



