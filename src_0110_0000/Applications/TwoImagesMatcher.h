
#ifndef TWOIMAGESMATCHER_H
#define TWOIMAGESMATCHER_H


#include "Pipeline.h"

class TwoImagesMatcher
{
	string imagename1;
	string imagename2;
	Pipeline * learningPipeline;
	Pipeline * matcherPipeline;


public:
	TwoImagesMatcher(string & image1, string & image2){

		imagename1 = image1;
		imagename2 = image2;

	}

	inline void setPipelines(Pipeline * learningPipeline, Pipeline * matcherPipeline){
		this->learningPipeline = learningPipeline;
		this->matcherPipeline = matcherPipeline;

		// replace matcherPipeline.buffer with the learningPipeline.buffer to have one common buffer:
		matcherPipeline->replaceBufferWith(learningPipeline->shareBuffer());
	
	}


	inline void run(){

		Mat img1 = imread(imagename1);
		Mat img2 = imread(imagename2);

		
		if (img1.data == NULL || img2.data == NULL){
			cerr << "TwoImagesMatcher: Error: one of the images is null" << endl;
			return;
		}

		//imshow("lp", img1);
		//imshow("mp", img2);


		learningPipeline->setInputImage(img1);
		learningPipeline->processPipeline();

		displayOutput(learningPipeline, "learningPipeline");
		

		matcherPipeline->setInputImage(img2);
		matcherPipeline->processPipeline();

		displayOutput(matcherPipeline, "matcherPipeline");

		// 

		// need to show correctly images using imshow or cvShowImage
		cvWaitKey(0);

	}


protected:
	

	inline void displayOutput(Pipeline * pipeline, string name){
	
		// Show pipeline lines:
		for (int i=0; i<pipeline->getNumberOfOutputs();i++){
			if (pipeline->isLineShown(i)){
				stringstream ss;
				ss << i;
				string wname = "Pipeline_" + name +": " + ss.str();
				cvShowImage(wname.c_str(), &(pipeline->getOutputImage(i)).operator CvMat());
			}
		}
	}

};


#endif
