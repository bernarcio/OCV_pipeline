
#ifndef MOTIONDETECTOR_H
#define MOTIONDETECTOR_H

#include "CameraCapture_VideoInput.h"

#include "DynBckGrndSubtractionFilter.h"
#include "MorphoFilter.h"
#include "MathBinaryOpFilter.h"
#include "MotionHistoryFilter.h"
#include "ThresholdFilter.h"
#include "FIFOBufferFilter.h"



class MotionDetector
{
	// Camera:
	CameraCapture_VideoInput cameraCapture;
	
	// Pipeline:
	Pipeline pipeline;
	
	// Set keyboard handler:
	KeyboardHandle keyHandler;

	// filters:
	ImageAbstractFilter * dbsFilter;
	ImageAbstractFilter * motionHistoryFilter;
	ImageAbstractFilter * thresholdFilter;
	ImageAbstractFilter * logicalAND;
	ImageAbstractFilter * fifoBuffer;
	//ImageAbstractFilter * camshiftFilter;
		
public:

	MotionDetector(){
		
		// type 1
		//dbsFilter = new DynBckGrndSubtractionFilter(8, 4, 0.7, 1.);
		// type 2
		dbsFilter = new DynBckGrndSubtractionFilter(4, 20.0, false);

		//camshiftFilter = new CamShiftFilter();
	
		// arguments: 
		//		- buffer size, 4 (used to compare two frames: 1 and last modulo buffer size => create a silhouette. larger buffer size, bigger the history)
		//		- total time, 1.0 unit (used to save frames history with the current time value smaller than the total time. Larger total time, bigger the traces of the motion in mhi) 	
		//		- threshold, 30 (used in the comparision between two frames, create the silhoutte)
		motionHistoryFilter = new MotionHistoryFilter(2,0.8,20);
	
		logicalAND = new MathBinaryOpFilter(MathBinaryOpFilter::AND);
	
		thresholdFilter = new ThresholdFilter(ThresholdFilter::THRESH_BINARY, 150, 255);
		fifoBuffer = new FIFOBufferFilter(4);
	
	}
	
	inline void start()
	{
		init();
		help();
		run();
	}

protected:
	inline void help();
	
	inline void run(){
		cameraCapture.run();
	}
	
	inline void init();

};

inline void MotionDetector::help(){
	cout << "MotionDetector application" << endl;
	cout << "Usage: press 'p' to take picture and save it to HDD" << endl;
}

inline void MotionDetector::init(){
	
	// camera ->	GMM		-----T-- AND -> ?
	//		|__> historyMotion -/ 		
	pipeline.addSplit(1);
	pipeline.addFilter(1, dbsFilter);

	pipeline.addFilter(2, motionHistoryFilter);
	//pipeline.addFilter(2, thresholdFilter);

	PipelineInput in(1,2);
	pipeline.addFilter(in, logicalAND);
	pipeline.addFilter(1, fifoBuffer);

	//pipeline.addFilter(1, camshiftFilter);

	
	//...
	string path = "data/";
	keyHandler.addKey('p', KeyboardHandle::SAVE_IMAGE_ON_DISK, 1, path + "train_image.jpg");
	
	// Run camera capture:
	cameraCapture.setPipeline(&pipeline);
	cameraCapture.setKeyboardHandler(&keyHandler);
	
}


#endif