#ifndef MASKGENERATIONFILTER_H
#define MASKGENERATIONFILTER_H

#include "ImageAbstractFilter.h"

/*
	Generate binary image with zero background and geometrical figure (rectangle, ellipse) with 255

	x,y - center of ellipse or top-left point of the rectangle
	w,h - 
*/

class MaskGenerationFilter : public ImageAbstractFilter
{


public:
	enum MaskType {RECTANGLE, ELLIPSE};
	MaskGenerationFilter(MaskType maskType, int x, int y, int w, int h, double angle){
		point.x = x;
		point.y = y;
		size = Size(w,h);
		this->angle = angle;
		this->maskType = maskType;

		this->rect = RotatedRect(point, size, this->angle);
		
		loadFromBuffer = false;

	};

	MaskGenerationFilter(MaskType maskType, string nameOfBoundRect, bool topLeftAtZero=false){
		this->nameOfBoundRect = nameOfBoundRect;
		point.x = 0;
		point.y = 0;
		size = Size(1,1);
		this->angle = 0.0;
		this->maskType = maskType;
		this->rect = RotatedRect(point, size, this->angle);
		loadFromBuffer = true;
		this->topLeftAtZero=topLeftAtZero;
	};

	~MaskGenerationFilter(){};

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer); 

private:
	MaskType maskType;
	Point point;
	Size size;
	double angle;
	RotatedRect rect;
	string nameOfBoundRect;
	bool loadFromBuffer;
	bool topLeftAtZero;

};



inline void MaskGenerationFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	Mat img = buffer->getOutputImage(input.getChannelNumber());
	Mat mask = Mat::zeros(Size(img.cols, img.rows),CV_8U);

	if (loadFromBuffer){
		Rect *r = buffer->getInternalElement<Rect>(nameOfBoundRect);
		if (r==NULL){
			return;
		}else{
			// if reference image of the rectangle is not the same as the one of the current image:
			int x; 
			int y; 
			if (topLeftAtZero){
				x=0;
				y=0;
			}else{
				x = (*r).x;
				y = (*r).y;
			}
			rect = RotatedRect(Point2f(x + 0.5*(*r).width, y + 0.5*(*r).height),Size((*r).width, (*r).height), angle);
		}
	}

	if (maskType == ELLIPSE){
		ellipse(mask, rect, Scalar(255,255,255),-1);
	}else if (maskType == RECTANGLE){
		rectangle(mask, rect.boundingRect(), Scalar(255,255,255),-1);
	}


	// return :
	buffer->setOutputImages(mask, input.getChannelNumber());

}



#endif