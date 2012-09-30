

#ifndef CAMERASIMPLECAPTURE_H
#define CAMERASIMPLECAPTURE_H


#include <opencv2/opencv.hpp>
#include "Pipeline.h"

using namespace cv;
using namespace std;


class CameraSimpleCapture
{

	VideoCapture capture;
	Pipeline * pipeline;

public:
	CameraSimpleCapture(Pipeline * p=0){
		pipeline = p;
	}

	void setPipeline(Pipeline * p) {pipeline = p;}
	void run(); // Method to start acquisition

};


void CameraSimpleCapture::run()
{
	capture.open(0);
	if (!capture.isOpened()){
		cerr << "Camera load error" <<endl;
	}

	//namedWindow("Acquisition");
	
	Mat testImage;
	capture >> testImage;

	// disable auto gain & exposure:
	capture.set(CV_CAP_PROP_AUTO_EXPOSURE, 0.0);
	cerr <<" CV_CAP_DSHOW: " << capture.get(CV_CAP_DSHOW) << endl;
	

	

	cout << "Camera acquisition is started. Press 'q' to quit. " << endl;

	cout << "Camera gain is configured using keys 'o' (+), 'p' (-). " << endl;
	cout << "Camera exposure is configured using keys 'k' (+), 'l' (-). " << endl;


	while(1){
	
		Mat img; 
		//CvMat * small_img; 
		capture >> img;
		

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

		// keyboard handling :
		int c = cvWaitKey(5);
		if( (char) c == 'q' ) { break; } 

		// FPS: 
		// cout << "fps: " << (double) capture.get(CV_CAP_PROP_FPS) << endl;

		// Camera gain & exposure configuration:
		if ( (char) c == 'o') { 
			double v = capture.get(CV_CAP_PROP_GAIN);
			capture.set(CV_CAP_PROP_GAIN, v + 10);
			cout << "Camera gain is increased : " << capture.get(CV_CAP_PROP_GAIN) << endl;
		}
		if ( (char) c == 'p') { 
			double v = capture.get(CV_CAP_PROP_GAIN);
			capture.set(CV_CAP_PROP_GAIN, v - 10);
			cout << "Camera gain is decreased : " << capture.get(CV_CAP_PROP_GAIN) << endl;
		}
		if ( (char) c == 'k') { 
			double v = capture.get(CV_CAP_PROP_EXPOSURE);
			capture.set(CV_CAP_PROP_EXPOSURE, v + 10);
			cout << "Camera exposure is increased : " << capture.get(CV_CAP_PROP_EXPOSURE) << endl;
		}
		if ( (char) c == 'l') { 
			double v = capture.get(CV_CAP_PROP_EXPOSURE);
			capture.set(CV_CAP_PROP_EXPOSURE, v - 10);
			cout << "Camera exposure is decreased : " << capture.get(CV_CAP_PROP_EXPOSURE) << endl;
		}
		
	}


}


#endif
