OCV_pipeline
============

OpenCV filters are managed using a pipeline for webcam image processsing. VideoInput library is used for camera configuration.

Idea is to use a wrapper over OpenCV functionalities as a chain of filters:
Example:

/**
* -------------------------------------------------------------------------------------------------------
*/

#include "CameraCapture_VideoInput.h"
#include "MotionDetectionFilter.h"
#include "HistMotionDetectionFilter.h"
#include "MathBinaryOpFilter.h"
#include "MorphoFilter.h"

int main(void)
{
	// camera handling choice:
	CameraCapture_VideoInput cameraCapture;
	
	// filters:
	ImageAbstractFilter * motionDetectionFilter = new MotionDetectionFilter();
	ImageAbstractFilter * histMotionDetectionFilter = new HistMotionDetectionFilter();
	ImageAbstractFilter * logicalAND = new MathBinaryOpFilter(MathBinaryOpFilter::AND);
	ImageAbstractFilter * morphoClose = new MorphoFilter(3, MORPH_ELLIPSE, MORPH_CLOSE);
	ImageAbstractFilter * morphoOpen = new MorphoFilter(3, MORPH_ELLIPSE, MORPH_OPEN);
	

	// Pipeline:
	Pipeline pipeline;

	pipeline.addSplit(1);
	pipeline.addFilter(1, motionDetectionFilter);
	pipeline.addFilter(2, histMotionDetectionFilter);

	PipelineInput in(1,2);
	pipeline.addFilter(in, logicalAND);

	pipeline.addFilter(1, morphoOpen);
	pipeline.addFilter(1, morphoClose);


          // Set keyboard handler:
	KeyboardHandle keyHandler;
	string path = "data/";
	keyHandler.addKey('p', KeyboardHandle::SAVE_MATRIXDATA_ON_DISK, 0, path + "template_image.yml");
	keyHandler.addKey('p', KeyboardHandle::SAVE_IMAGE_ON_DISK, 0, path + "template_image.jpg");
	keyHandler.addKey('p', KeyboardHandle::SAVE_MATRIXDATA_IN_BUFFER, 0, "templ");
	keyHandler.addKey('l', KeyboardHandle::LOAD_MATRIXDATA_FROM_DISK_INTO_BUFFER, "templ", path + "template_image.yml");
	
	// Run camera capture:
	cameraCapture.setPipeline(&pipeline);
	cameraCapture.setKeyboardHandler(&keyHandler);
	cameraCapture.run();
	
	return 0;
}
-------------------------------------------------------------------------------------------------------

