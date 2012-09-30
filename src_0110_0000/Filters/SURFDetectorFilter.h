#ifndef SURFDETECTORFILTER_H
#define SURFDETECTORFILTER_H

#include "ImageAbstractFilter.h"


class SURFDetectorFilter : public ImageAbstractFilter
{

    SurfFeatureDetector featureDetector;
    SurfDescriptorExtractor descriptorExtractor;

public:
    SURFDetectorFilter(int nOctaves=4, int nOctaveLayers=2, bool extended=false) :
        featureDetector(400, nOctaves, nOctaveLayers),
        descriptorExtractor(nOctaves, nOctaveLayers, extended)
    {
    };
	~SURFDetectorFilter(){};


    inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};




void SURFDetectorFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	Mat inputImage = buffer->getOutputImage(input.getChannelNumber());
	if (inputImage.channels() == 3)
		cvtColor(inputImage,inputImage, CV_BGR2GRAY, 0);

	vector<KeyPoint> keyPoints;
    featureDetector.detect(inputImage, keyPoints, Mat());

	// draw keypoints:
	drawKeypoints(buffer->getOutputImage("orig"), keyPoints, buffer->getOutputImage("orig"), Scalar(255,0,0));

    // extract descriptors:
    Mat descriptors;
    descriptorExtractor.compute(inputImage, keyPoints, descriptors);

    // save keypoints into buffer:
    buffer->setInternalKeyPoints("SURFKeyPoints", keyPoints);
    buffer->setInternalImage("descriptors", descriptors);

}


#endif
