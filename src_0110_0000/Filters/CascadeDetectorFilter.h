
#ifndef CASCADEDETECTORFILTER_H
#define CASCADEDETECTORFILTER_H

#include "ImageAbstractFilter.h"

enum CascadeType {FACE};

class CascadeDetectorFilter : public ImageAbstractFilter
{

	CascadeClassifier cascade;
	CascadeType type;
	string cascadeName;
	bool status; // true if classifier is OK (cascade is loaded), false otherwise
	int scale;
	int freq; // parameter defines at what number of calls detector is called: freq = 1, detector called every time, freq = 3, detector is called once every 3 calls. 
	int count; // counter for freq.

	vector<Rect> faces; // detected objects
	Mat smallImg; // resized camera image 

public:
	CascadeDetectorFilter(int _freq=10, CascadeType _type=FACE, int sc=1) : type(_type){
		 
		cascadeName = "";
		if (type==FACE){
			cascadeName = "src/FiltersData/haarcascade_frontalface_alt.xml";
		}

		if(cascade.load( cascadeName ))
			status = true;
		else{
			status = false;
			cerr << "!!! CascadeClassifier was not loaded" << endl;
		}

		scale = sc;
		freq = _freq;
		count=0;
		
	};

	~CascadeDetectorFilter(){};


    inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};



inline void CascadeDetectorFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	if (status){

		// if it is 1 call of 'freq' number of calls -> proceed
		if (count == 0 || faces.size() < 1){

			Mat img = buffer->getOutputImage(input.getChannelNumber());
			Mat gray;
			smallImg = Mat( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );
			cvtColor( img, gray, CV_BGR2GRAY );

			resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
			equalizeHist( smallImg, smallImg );

	
			//CascadeClassifier method : detectMultiScale(const Mat& image, vector<Rect>& objects, double
			//					scaleFactor=1.1, int minNeighbors=3, int flags=0,
			//					Size minSize=Size(), Size maxSize=Size())
			cascade.detectMultiScale( smallImg, faces,
				1.1, 3, 0
				//|CV_HAAR_FIND_BIGGEST_OBJECT
				//|CV_HAAR_DO_ROUGH_SEARCH
				|CV_HAAR_SCALE_IMAGE
				,
				Size(30, 30)); 

			count++;
		}else{
			count++;
			(count >= freq) ? count = 0 : count;			
		}
		
		// draw rectangles for detected objects:
		if (faces.size()>0){

			InternalVariable n((int) faces.size());
			buffer->setInternalVariable("det_obj_num", n);
			int count=0;
			Rect nr;
			double fx = 0.15, fy = 0.25;
			for( vector<Rect>::iterator r = faces.begin(); r != faces.end(); r++)
			{
				Mat smallImgROI;
				// correction to scale:
				nr = *r; 
				nr.x *= scale; nr.y *= scale; 
				nr.width *= scale; nr.height *= scale;

				// enlarge detected rectangle:
				nr.width += (int) (fx*nr.width);
				nr.height += (int) (fy*nr.height);
				nr.y -= (int) (0.5*fy*nr.height);
				smallImgROI = smallImg(*r);

				// draw the rectangle:
				rectangle(buffer->getOutputImage("orig"), nr, Scalar(255,20,0));
		
				// save ROI into buffer:
				stringstream ss;
				ss << count;
				string name = "det_obj_rect_" + ss.str();
				Mat m(4,1,CV_32F,Scalar(0));
				m.at<float>(0,0) = nr.x;
				m.at<float>(1,0) = nr.y;
				m.at<float>(2,0) = nr.width;
				m.at<float>(3,0) = nr.height;
				buffer->setInternalImage(name, m);
				count++;
			}		
		
		}
		

	}

}


#endif