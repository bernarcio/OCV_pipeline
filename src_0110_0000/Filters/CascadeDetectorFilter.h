
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

	//vector<Rect> faces; // detected objects

public:
	CascadeDetectorFilter(CascadeType _type=FACE, int sc=1) : type(_type){
		 
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
		
	};

	~CascadeDetectorFilter(){};


    inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};



inline void CascadeDetectorFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{

	vector<Rect> faces; // detected objects

	if (status){

		
		if ( faces.size() < 1){

			Mat img = buffer->getOutputImage(input.getChannelNumber());
			Mat gray;
			Mat smallImg = Mat( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );
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
		}
		
		// draw rectangles for detected objects:
		if (faces.size()>0){

			InternalVariable n((int) faces.size());
			buffer->setInternalVariable("det_obj_num", n);
			int count=0;
			Rect nr;
			double fx = 0.05, fy = 0.35;
			Mat img = buffer->getOutputImage("orig");
			for( vector<Rect>::iterator r = faces.begin(); r != faces.end(); r++)
			{

				

				// correction to scale:
				(*r).x *= scale; (*r).y *= scale; 
				(*r).width *= scale; (*r).height *= scale;

				// enlarge detected rectangle:
				(*r).x = ((*r).x - (int) (0.5*fx*(*r).width) < 0) ? 0 : (*r).x - (int) (0.5*fx*(*r).width);
				(*r).y = ( (*r).y - (int) (0.5*fy*(*r).height) < 0 ) ? 0 : (*r).y - (int) (0.5*fy*(*r).height);

				(*r).width = ((*r).x + (*r).width + (int) (fx*(*r).width) >= img.cols) ? img.cols - (*r).x - 1 : (*r).width + (int) (fx*(*r).width);
				(*r).height = ((*r).y + (*r).height + (int) (fy*(*r).height) >= img.rows) ? img.rows - (*r).y - 1 : (*r).height + (int) (fy*(*r).height);
				
				nr = *r;

				// draw the rectangle:
				rectangle(img, nr, Scalar(255,20,0));
		
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

			// return 1st 'object' in the pipeline output:
			img = img(faces[0]);
			buffer->setOutputImages(img,input.getChannelNumber());
		
		}
		

	}

}


#endif