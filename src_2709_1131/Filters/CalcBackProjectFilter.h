/*
#ifndef CALCBACKPROJECT_H
#define CALCBACKPROJECT_H

#include "ImageAbstractFilter.h"

class CalcBackProjectFilter : public ImageAbstractFilter
{

	vector<vector<Mat> > trainingImagesForObjects; // e.g. for 1st object define 10 trainingImages, for 2nd object define same 10 other trainingImages
	int numberOfTrainingImages; // for all objects
	vector<int> counter;
	bool useInternalImages;

public: 
	CalcBackProjectFilter(bool _useInternalImages=true)
	{
		// initialize parameters for one object:  more objects can be for internal images
		numberOfTrainingImages = 10;
		vector<Mat> trainingImages;
		trainingImages.get_allocator().allocate(numberOfTrainingImages);
		trainingImagesForObjects.push_back(trainingImages);
		counter.push_back(0);

		useInternalImages = _useInternalImages;
	};
	
	~CalcBackProjectFilter(){};

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);




};

inline void CalcBackProjectFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	
	if (useInternalImages){
		// import images from CascadeDetector:
		int numberOfObjects = buffer->getInternalVariable("det_obj_num").GetIntValue();
		for (int i=0;i<numberOfObjects;i++){
			stringstream ss;
			ss << i;
			string name = "det_obj_" + ss.str();
			
			if (trainingImagesForObjects.size() > i){
				vector<Mat> trainingImages;
				trainingImages.get_allocator().allocate(numberOfTrainingImages);
				trainingImagesForObjects.push_back(trainingImages);
				counter.push_back(0);
			}
			trainingImagesForObjects[i][counter[i]] = buffer->getInternalImage(name);
			counter[i] = (counter[i] < numberOfTrainingImages) ? counter[i]+1 : 0;
			
		
			// void calcBackProject(const Mat* arrays, int narrays, const int* channels, InputArray hist, OutputArray
			//						backProject, const float** ranges, double scale=1, bool uniform=true )
			//calcBackProject
		
		}

		

	}else{
		trainingImagesForObjects[0][counter[0]] = buffer->getOutputImage(input.getChannelNumber());
		counter[0] = (counter[0] < numberOfTrainingImages) ? counter[0]+1 : 0;	
	



		// void calcBackProject(const Mat* arrays, int narrays, const int* channels, InputArray hist, OutputArray
		//						backProject, const float** ranges, double scale=1, bool uniform=true )
		calcBackProject(&trainingImagesForObjects[0][0], counter[0], )
		//cv::
	
	}




	



}


#endif
*/