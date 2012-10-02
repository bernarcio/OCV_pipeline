#ifndef VIDEOREADER_H
#define VIDEOREADER_H


#include <opencv2/opencv.hpp>
#include "Pipeline.h"

using namespace cv;
using namespace std;


class VideoReader
{
	VideoCapture capture;
	Pipeline * pipeline;
	string filename;

public:
	VideoReader(){
	
	}

	~VideoReader(){
		
	}

	inline void setPipeline(Pipeline * p) {pipeline = p;}
	inline void run(); // Method to start acquisition with pipeline processing
	inline void setFile(string name){ filename = name; };

protected:
	inline bool captureInit();
};


inline bool VideoReader::captureInit()
{
		capture.open(filename);
		if (!capture.isOpened()){
			return false;	
		}

}

inline void VideoReader::run()
{

	if(!captureInit()){
		return;
	}

	while (1){
		Mat img; 
		capture >> img;
		if (img.data == NULL){
			cout << "That's all folks" << endl;
			break;
		}

		// Pipeline image processing:
		pipeline->setInputImage(img);
		pipeline->processPipeline();

		// Show pipeline lines:
		for (int i=0; i<pipeline->getNumberOfOutputs();i++){
			if (pipeline->isLineShown(i)){
				Mat pipelineOutImage(pipeline->getOutputImage(i));		
				stringstream ss;
				ss << i;
				string wname = "Pipeline_" + ss.str();
				cvShowImage(wname.c_str(), &pipelineOutImage.operator CvMat());
			}
		}


		// keyboard handling : 
		int c = cvWaitKey(5);
		if( (char) c == 'q' || (char) c == 27 ) { break; } 
		
	}


}


#endif