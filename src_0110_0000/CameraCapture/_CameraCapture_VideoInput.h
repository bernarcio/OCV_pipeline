

#pragma once

#include <opencv2/opencv.hpp>
#include "Pipeline.h"

// use special library for video input configurations
#include "videoInput.h"

using namespace cv;
using namespace std;


class CameraCapture_VideoInput
{
	videoInput VI;
	int device;
	int numDevices;
	Mat currentImage;
	Pipeline * pipeline;

public:
	CameraCapture_VideoInput(Pipeline * p=0){
		pipeline = p;
		// list of opened devices
		device = 0;
		numDevices = videoInput::listDevices();	
		VI.setUseCallback(false);
	}
	~CameraCapture_VideoInput(){
		//Shut down devices properly
		VI.stopDevice(device);
	}

	void setPipeline(Pipeline * p) {pipeline = p;}
	void run(); // Method to start acquisition

};


void CameraCapture_VideoInput::run()
{
	
	

	if (!VI.setupDevice(device, VI_COMPOSITE))
			cerr << "Camera load error" <<endl; 

	

	int width = VI.getWidth(device);
	int height = VI.getHeight(device);
		
	//namedWindow("Acquisition");
	

	cout << "Camera acquisition is started. Press 'q' to quit. " << endl;
	cout << "Press 's' for camera settings configuration" << endl;

	while(1){
	
		Mat img(height, width, CV_8UC3);
		if(VI.isFrameNew(device)){
			VI.getPixels(device, img.data, false, true);	//fills pixels as a BGR (for openCV) unsigned char array - no flipping
		}
	
		

		// Display:
		// mirror transformation:
		// flip(img,img,1);
		// cvShowImage("Acquisition", &img.operator CvMat());
		

		// Pipeline image processing:
		pipeline->setInputImage(img);
		pipeline->processPipeline();

		//rectangle(pipelineOutImage, Rect(10,10,100,100), Scalar(0,0,255), 3);

		for (int i=0; i<pipeline->getNumberOfOutputs();i++){
			Mat pipelineOutImage(pipeline->getOutputImage(i));		
			stringstream ss;
			ss << i;
			string wname = "Pipeline_" + ss.str();
			cvShowImage(wname.c_str(), &pipelineOutImage.operator CvMat());
		}

		//imshow("mhi",pipeline->getBufferImage("mhi"));

		// keyboard handling :
		int c = cvWaitKey(5);
		if( (char) c == 'q' || (char) c == 27 ) { break; } 
		
		if( (char) c == 's' ) { 
			VI.showSettingsWindow(device);
		} 

		
	}


}
