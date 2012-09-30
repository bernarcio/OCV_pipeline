#ifndef MOTIONDETECTIONFILTER_H
#define MOTIONDETECTIONFILTER_H

#include "ImageAbstractFilter.h"
#include "Pipeline.h"

// Method taken from P.Valiere internship report:
// 
//	A = I(t) - I(t-1)
//	B = I(t-1) - I(t-2)
//
//  S = 1.0
//	binarization condition :  S_A = ( abs(A - mean(A))/sqrt(var(A)) > S  )
//			     			  S_B = ( abs(B - mean(B))/sqrt(var(B)) > S  )
//
//  Output = S_A OR S_B


class MotionDetectionFilter : public ImageAbstractFilter
{
	vector<Mat> buffer;
	int count;
	Pipeline pipeline;
public:

	MotionDetectionFilter(){
		count = 0;

	};
	~MotionDetectionFilter(){};

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};


inline void MotionDetectionFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * pipelineBuffer)
{
	// fill-in the buffer:
	if (buffer.size() < 3){
		buffer.push_back(pipelineBuffer->getOutputImage(input.getChannelNumber()));
		count++;
		if (count<3)
			return;
	}else{
		buffer[count % 3] = pipelineBuffer->getOutputImage(input.getChannelNumber());
		count++;
	}

	// define A, B matrices:
	int ind3 = (count-1) % 3;
	int ind2 = (count-2) % 3;
	int ind1 = (count-3) % 3;
	Mat A = buffer[ind3] - buffer[ind2];
	Mat B = buffer[ind2] - buffer[ind1];
	if (A.channels() == 3){
		cvtColor(A,A,CV_BGR2GRAY);
		cvtColor(B,B,CV_BGR2GRAY);
	}
	//imshow("A", A);
	//imshow("B", B);


	// Binarize images:
		// compute mean and variance:
	Scalar  mnA, mnB, stddevA, stddevB; 
	meanStdDev(A, mnA, stddevA);
	meanStdDev(B, mnB, stddevB);
		// binarize:
	double s=1.0;
	double meanA=mnA.val[0], meanB=mnB.val[0];
	//cerr << "means: " << meanA << ", " << meanB << endl;
	double sigA=stddevA.val[0], sigB=stddevB.val[0];

	Mat Sa(A.rows, A.cols, CV_8U, mnA);
	Mat Sb(B.rows, B.cols, CV_8U, mnB);
	absdiff(A,Sa,Sa);
	Sa = (1./sigA)*Sa;

	absdiff(B,Sb,Sb);
	Sb = (1./sigB)*Sb;
	threshold(Sa,Sa,s,255,CV_THRESH_BINARY);
	threshold(Sb,Sb,s,255,CV_THRESH_BINARY);
	
//	imshow("SA",Sa);
//	imshow("SB",Sb);

	// S_A logic OR S_B
	Mat output = Sa | Sb;
	
	// return:
	pipelineBuffer->setOutputImages(output,input.getChannelNumber());



}


#endif