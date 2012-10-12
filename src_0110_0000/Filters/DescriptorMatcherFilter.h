#ifndef DESCRIPTORMATCHERFILTER_H
#define DESCRIPTORMATCHERFILTER_H


#include "ImageAbstractFilter.h"


class DescriptorMatcherFilter : public ImageAbstractFilter
{

    //FlannBasedMatcher matcher;
    BruteForceMatcher<L1<float> > matcher;
    int knnValue;
    bool knnMatch;
    bool isTrained;
    bool descriptorAdded;

public:
    DescriptorMatcherFilter(bool KnnMatch=false, int KnnValue=0){
        knnMatch = KnnMatch;
        knnValue = KnnValue;
        isTrained = false;
        descriptorAdded = false;
    }
    ~DescriptorMatcherFilter(){};


    inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);


#ifdef Q_WS_W32
    inline void Add(const QVector<Mat>& descriptors) {
        matcher.add(descriptors.toStdVector());
        descriptorAdded = true;
    }
#endif

	inline void Add(const vector<Mat>& descriptors) {
        matcher.add(descriptors);
        descriptorAdded = true;
    }


    inline void Train(){
        matcher.train();
        isTrained = true;
    }

    inline void Clear(){
        matcher.clear();
        descriptorAdded = false;
        isTrained = false;
    }

};

inline void DescriptorMatcherFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{

    if (!descriptorAdded){
        cout << " !!! No trained descriptors available. exit filter" << endl;
        return;
    }
    if (!isTrained)
        Train();

    vector<DMatch> matches;
    vector<vector<DMatch> > knnmatches;
    // get query descriptors:
    Mat queryDescriptors, *qd = buffer->getInternalImage("descriptors");
	if (qd !=NULL){
		queryDescriptors = *qd;
	}else{
		cerr << "DescriptorMatcherFilter: Mat was not found at name : " << "descriptors" << endl;
		return;
	}

    bool matchesFound = false;

    if (!knnMatch){

        //matcher.match(queryDescriptors, matches);
        matcher.radiusMatch(queryDescriptors, knnmatches, 0.1);

        if (!knnmatches.empty())
            matchesFound = true;
    }else{
        matcher.knnMatch(queryDescriptors, knnmatches, knnValue, Mat());
        if (!knnmatches.empty())
            matchesFound = true;
    }


    cout << "radius matches size : " << knnmatches.size() << endl;
    if (knnmatches.size()>0)
        cout << "radius matches 2nd size of 0 : " << knnmatches[0].size() << endl;


//    if (matchesFound){
//        //cout << "matchesFound : " << matchesFound << endl;
//        // find matched keypoints :
//        vector<KeyPoint> keyPoints = buffer->getInternalKeyPoints("SURFKeyPoints");
//        vector<KeyPoint> matchedKeyPoints;
//        if (!knnMatch){
//            vector<DMatch>::iterator it = matches.begin();
//            for (;it!=matches.end();it++){
//                int index = (*it).queryIdx;
//                matchedKeyPoints.push_back(keyPoints[index]);
//            }
//        }

//        // draw keypoints:
//        drawKeypoints(buffer->getOutputImage(1), matchedKeyPoints, buffer->getOutputImage(1), Scalar(0,255,0));
//    }


}


#endif // DESCRIPTORMATCHERFILTER_H
