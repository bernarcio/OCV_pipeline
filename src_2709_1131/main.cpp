

//#include "MotionDetector.h"
//#include "blurfilter.h"
//imageabstractfilter * filter1 = new blurfilter();
//imageabstractfilter * filter2 = new blurfilter();
//imageabstractfilter * filter3 = new blurfilter();
//void f(){
//	pipeline pipeline;
//	pipeline.addfilter(1, filter1);
//	pipeline.addfilter(1, filter2);
//	pipeline.addfilter(1, filter3);
//	// for vector<object *>  when vector is destroyed, elements are not released. -> memory leak !
//}

// Motion Detector:
//int main(void)
//{
//
//	// TODO: replace CamShift by BlobDetector or smth similar
//
//	MotionDetector motionDetector;
//	motionDetector.start();
//
//	//system("PAUSE");
//	return 0;
//}




// test main:
#include "CameraCapture_VideoInput.h"

#include "MatchTemplateFilter.h"
#include "ContourFilter.h"
#include "MotionDetectionFilter.h"
#include "HistMotionDetectionFilter.h"
#include "MathBinaryOpFilter.h"
#include "MorphoFilter.h"

int main(void)
{


	// camera handling choice:
	CameraCapture_VideoInput cameraCapture;
	
	// filters:
	//ImageAbstractFilter * matchTemplateFilter = new MatchTemplateFilter(MatchTemplateFilter::SQDIFF_NORMED);
	ImageAbstractFilter * cannyFilter = new ContourFilter(3,30,150);
	ImageAbstractFilter * motionDetectionFilter = new MotionDetectionFilter();
	ImageAbstractFilter * histMotionDetectionFilter = new HistMotionDetectionFilter();
	ImageAbstractFilter * logicalAND = new MathBinaryOpFilter(MathBinaryOpFilter::AND);
	ImageAbstractFilter * morphoClose = new MorphoFilter(3, MORPH_ELLIPSE, MORPH_CLOSE);
	ImageAbstractFilter * morphoOpen = new MorphoFilter(3, MORPH_ELLIPSE, MORPH_OPEN);
	

	// Pipeline:
	Pipeline pipeline;

//	pipeline.addSplit(1);
//	pipeline.addFilter(1, motionDetectionFilter);

//	pipeline.addFilter(2, histMotionDetectionFilter);
	//pipeline.addFilter(1, cannyFilter);

	pipeline.addFilter(1, histMotionDetectionFilter);

//	PipelineInput in(1,2);
//	pipeline.addFilter(in, logicalAND);

	
	//pipeline.addFilter(1, matchTemplateFilter);

	//
	pipeline.addFilter(1, morphoOpen);
	pipeline.addFilter(1, morphoClose);

	//pipeline.addSplit(1);

	//pipeline.addFilter(1, segmentMotionFilter);

	
	//pipeline.addFilter(2, faceDetector);
	//pipeline.addFilter(2, morphoErode);


	// Set keyboard handler:
	KeyboardHandle keyHandler;

	//...
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






/*
#include "CameraSimpleCapture.h"
#include "CameraCapture_VideoInput.h"

#include "BlurFilter.h"
#include "DynBckGrndSubtractionFilter.h"
#include "MorphoFilter.h"
#include "CamShiftFilter.h"
#include "ImproveResolutionFilter.h"
#include "MedianFilter.h"
#include "ResizeFilter.h"
#include "RescaleFilter.h"
#include "ContourFilter.h"

#include "MathBinaryOpFilter.h"

#include "MotionHistoryFilter.h"
#include "SegmentMotionFilter.h"

#include "CascadeDetectorFilter.h"
*/
/*
#include "ThresholdFilter.h"
#include "FIFOBufferFilter.h"

#include "FourierTransformFilter.h"

#include "SURFDetectorFilter.h"
#include "DescriptorMatcherFilter.h"
*/





	/*
	// definitions:
	// camera handling choice:
	CameraCapture_VideoInput cameraCapture;
		
	
	// filters"
	ImageAbstractFilter * newResolutionFilter = new ImproveResolutionFilter(4);

	ImageAbstractFilter * rescaleFilter = new RescaleFilter(10./255);

	ImageAbstractFilter * resizeFilter = new ResizeFilter(2, INTER_LINEAR);
	ImageAbstractFilter * medianFilter = new MedianFilter(3);

	ImageAbstractFilter * blurFilter = new BlurFilter(3,3);
	
	ImageAbstractFilter * cannyFilter = new ContourFilter(3,30,150);
	ImageAbstractFilter * sobelFilter = new ContourFilter(3);

	// type 1
	//ImageAbstractFilter * dbsFilter = new DynBckGrndSubtractionFilter(8, 4, 0.7, 1.);
	// type 2
	ImageAbstractFilter * dbsFilter = new DynBckGrndSubtractionFilter(4, 20.0, false);

	ImageAbstractFilter * morphoClose = new MorphoFilter(3, MORPH_ELLIPSE, MORPH_CLOSE);
	ImageAbstractFilter * morphoOpen = new MorphoFilter(3, MORPH_ELLIPSE, MORPH_OPEN);
	ImageAbstractFilter * morphoErode = new MorphoFilter(1, MORPH_ELLIPSE, MORPH_ERODE);
	ImageAbstractFilter * camshiftFilter = new CamShiftFilter();
	
	// arguments: 
	//		- buffer size, 4 (used to compare two frames: 1 and last modulo buffer size => create a silhouette. larger buffer size, bigger the history)
	//		- total time, 1.0 unit (used to save frames history with the current time value smaller than the total time. Larger total time, bigger the traces of the motion in mhi) 	
	//		- threshold, 30 (used in the comparision between two frames, create the silhoutte)
	ImageAbstractFilter * motionHistoryFilter = new MotionHistoryFilter(2,0.8,20);
	ImageAbstractFilter * segmentMotionFilter = new SegmentMotionFilter();
	
	ImageAbstractFilter * faceDetector = new CascadeDetectorFilter(100);
	
	ImageAbstractFilter * logicalAND = new MathBinaryOpFilter(MathBinaryOpFilter::AND);
	
	ImageAbstractFilter * thresholdFilter = new ThresholdFilter(ThresholdFilter::THRESH_BINARY, 150, 255);
	ImageAbstractFilter * fifoBuffer = new FIFOBufferFilter(4);
	

	ImageAbstractFilter * surfDetector = new SURFDetectorFilter();
	
	DescriptorMatcherFilter * matcher = new DescriptorMatcherFilter();


	// DOES NOT WORK CORRECTLY: ---------
	//ImageAbstractFilter * dftFilter = new FourierTransformFilter(false, true);
	//ImageAbstractFilter * idftFilter = new FourierTransformFilter(true, true);
	// ----------------------------------
	
	// Pipeline:
	Pipeline pipeline;
	
	
	
	// camera ->	GMM		-----T-- AND -> ?
	//		|__> historyMotion -/ 		
	pipeline.addSplit(1);

//	pipeline.addFilter(1, morphoErode);
		
	
	
	pipeline.addFilter(1, dbsFilter);
	

	pipeline.addFilter(2, motionHistoryFilter);
	pipeline.addFilter(2, thresholdFilter);

	PipelineInput in(1,2);
	pipeline.addFilter(in, logicalAND);

	pipeline.addFilter(1,fifoBuffer);

	pipeline.addFilter(1, surfDetector);





	// DOES NOT WORK CORRECTLY: ---------
	//pipeline.addFilter(1,dftFilter);
	//pipeline.addFilter(1,idftFilter);
	// -----------------------------------



	//pipeline.addFilter(1, camshiftFilter);


	//
	//pipeline.addFilter(1, morphoOpen);
	//pipeline.addFilter(1, morphoClose);

	//pipeline.addSplit(1);

	//pipeline.addFilter(1, segmentMotionFilter);

	
	//pipeline.addFilter(2, faceDetector);
	//pipeline.addFilter(2, morphoErode);


	// Set keyboard handler:
	KeyboardHandle keyHandler;

	//...
	string path = "data/";
	//keyHandler.addKey('p', KeyboardHandle::SAVE_IMAGE_ON_DISK, 0, path + "image.jpg");
	//keyHandler.addKey('i', KeyboardHandle::SAVE_MATRIXDATA_ON_DISK, 0, path +  "train_matrix");
	//keyHandler.addKey('o', KeyboardHandle::SAVE_MATRIXDATA_IN_BUFFER, 0, "train_image");
	//keyHandler.addKey('l', KeyboardHandle::LOAD_MATRIXDATA_FROM_DISK_INTO_BUFFER, "train_matrix", path + "train_matrix.yml");
	
	keyHandler.addKey('p', KeyboardHandle::SAVE_IMAGE_ON_DISK, 0, path + "train_image.jpg");
	keyHandler.addKey('p', KeyboardHandle::SAVE_MATRIXDATA_ON_DISK, "descriptors", path + "train_descriptors.yml");
	keyHandler.addKey('l', KeyboardHandle::LOAD_MATRIXDATA_FROM_DISK_INTO_BUFFER, "train_descriptors", path + "train_descriptors.yml");
	//keyHandler.addKey('t', matcher->Add, "train_descriptors");
	

	// Run camera capture:
	cameraCapture.setPipeline(&pipeline);
	cameraCapture.setKeyboardHandler(&keyHandler);
	cameraCapture.run();
	*/






// --------------------- SOME TESTS: -------------------------


//void display(Mat & A)
//{
//	cerr << "Display matrix: A = " << endl;
//	for (int i=0; i<A.rows; i++){
//		for (int j=0; j<A.cols;j++)
//			cerr << A.at<double>(i,j) << ", ";
//		cerr << endl;
//	}
//
//}
//
//void foo(Mat & A)
//{
//	A *= 3;
//	Mat B = A;
//	Mat C(A);
//	cerr << "Pointers C & B & A : " << &C << ", " << &B << ", " << &A << endl;
////	cerr << "Data Pointers C & B & A : " << C.data << ", " <<  B.data << ", " <<  A.data << endl;
//	
//	// Results: A, B, C has 3 different pointers but have the same pointer to the data
//
//}
//

//// function as argument:
//
//void func(int x, int (*f)(int)){
//	cout << "f(x): " << f(x) << endl;
//}
//
//
//
//int f2(int x){
//	return x*x;
//}
//
//
//// same with classes:
//class A
//{
//public:
//	int f2(int x){
//		return x*x;
//	}	
//};
//
//void func(int x, int (A::*f)(int)){
//	A a;
//	cout << "A::f(x): " << (a.*f)(x) << endl;
//}




	
	//A a;
	//func(2, f2);
	//func(2, &A::f2);


	/*
	// TESTS:
	Mat A = Mat::ones(4,3,CV_64F);
	Mat B = 2*A;
	Mat C = 3*A;
	Mat D = 4*A;
	display(A);

	Mat T = Mat(A.rows*2, A.cols*2, A.type(), Scalar(0));
	display(T);
	// double loop realisation:
	for (int i=0;i<A.rows;i++){
		for (int j=0;j<A.cols;j++){
			T.at<double>(2*i,2*j) = A.at<double>(i,j) + 0.01*(10*(i) + j);		
			T.at<double>(2*i+1,2*j) = B.at<double>(i,j) + 0.01*(10*(i) + j);
			T.at<double>(2*i,2*j+1) = C.at<double>(i,j) + 0.01*(10*(i) + j);
			T.at<double>(2*i+1,2*j+1) = D.at<double>(i,j) + 0.01*(10*(i) + j);
		}
	}
	display(T);
	
	cerr << "Display linear T: using pointers: " << endl;
	for (int i=0;i<T.rows*T.cols;i++){
		cerr << *((double*) (T.data) + i) << ", " << endl;
	}
	
	cerr << "steps: " << endl;
	cerr << T.step[0] << endl;
	cerr << T.step[1] << endl;
	cerr << T.step[0]/T.step[1] << endl;

	
	cerr << 0 % 3 << " = " << 0 << endl;
	cerr << 1 % 3 << " = " << 1 << endl;
	cerr << 2 % 3 << " = " << 0 << endl;
	cerr << 3 % 3 << " = " << 1 << endl;
	cerr << 4 % 3 << " = " << 0 << endl;

	cerr << 0 / 3 << " = " << 0 << endl;
	cerr << 1 / 3 << " = " << 1 << endl;
	cerr << 2 / 3 << " = " << 0 << endl;
	cerr << 3 / 3 << " = " << 1 << endl;
	cerr << 4 / 3 << " = " << 0 << endl;
	

	int step = T.step[0]/T.step[1];
	for (int i=0;i<A.rows;i++){
		for (int j=0;j<A.cols;j++){
			*((double*) (T.data) + step*(2*i) + 2*j) = A.at<double>(i,j) + 0.01*(10*(i) + j);		
			*((double*) (T.data) + step*(2*i+1) + 2*j) = B.at<double>(i,j) + 0.01*(10*(i) + j);
			*((double*) (T.data) + step*(2*i) + (2*j + 1)) = C.at<double>(i,j) + 0.01*(10*(i) + j);
			*((double*) (T.data) + step*(2*i+1) + (2*j + 1)) = D.at<double>(i,j) + 0.01*(10*(i) + j);
			
		}
	}

	display(T);
	
	Mat R; 
	Mat S = Mat(3,3,CV_32F, Scalar(0));


	if (R.data == NULL)
		cerr << "R says yes" << endl;
	else
		cerr << "R says no" << endl;

	if (S.data != NULL)
		cerr << "S says yes" << endl;
	else
		cerr << "S says no" << endl;
		

	//*/

