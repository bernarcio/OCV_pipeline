#ifndef MATCHTEMPLATEFILTER_H
#define MATCHTEMPLATEFILTER_H

#include "ImageAbstractFilter.h"

class MatchTemplateFilter : public ImageAbstractFilter
{


public:
	enum FilterType {SQDIFF, SQDIFF_NORMED, CCORR, CCORR_NORMED, CCOEFF, CCOEFF_NORMED};
	MatchTemplateFilter(FilterType t=SQDIFF, string tName="templ", bool replaceTempl=false, bool traceFromPrevPoint=false, string nameOfDetObj="detObj"){
		type = t;

		if (type == SQDIFF)
			method = CV_TM_SQDIFF;
		else if (type == SQDIFF_NORMED)
			method = CV_TM_SQDIFF_NORMED;
		else if (type == CCORR)
			method = CV_TM_CCORR;
		else if (type == CCORR_NORMED)
			method = CV_TM_CCORR_NORMED;
		else if (type == CCOEFF)
			method = CV_TM_CCOEFF;
		else if (type == CCOEFF_NORMED)
			method = CV_TM_CCOEFF_NORMED;

		templName = tName;

		this->replaceTempl = replaceTempl;
		this->traceFromPrevPoint = traceFromPrevPoint;
		this->prevPoint = Point(-1,-1);
		this->nameOfDetObj = nameOfDetObj;
	}

	~MatchTemplateFilter(){};
	

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

private:
	FilterType type;
	string templName; // name of the template to search Matrix in the pipeline buffer, by default "templ"
	int method;

	bool replaceTempl;
	bool traceFromPrevPoint;
	Point prevPoint;
	string nameOfDetObj;

};



inline void MatchTemplateFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	Mat result, templ, *tp = buffer->getInternalImage(templName);
	if (tp == NULL)
		return;
	else{
		templ = *tp;
		if (templ.channels() == 3)
			cvtColor(templ, templ, CV_BGR2GRAY);
	}
	
	Mat inputImage = buffer->getOutputImage(input.getChannelNumber());
	if (inputImage.channels() == 3)
		cvtColor(inputImage, inputImage, CV_BGR2GRAY);
	
	
	

	// match template:
	matchTemplate(inputImage, templ, result, method);

	// return:
	// transform to 8U and invert:
	//result.convertTo(result, CV_8U,255);
	//result = 255 - result;

	//int res = (int) (255*result.at<float>(0,0));
	//cerr << "result = (8U) and (32F)" << res << ", " << result.at<float>(0,0) << endl;
	//Mat output(inputImage.rows, inputImage.cols, CV_8U, Scalar(res));

	// find the minimum:
	double threshVal = 0.3; //
	double minVal;
	Point minPoint;
	minMaxLoc(result,&minVal, 0, &minPoint,0);
	//cerr << "Min is " << minVal << endl;
	//cerr << "Min is at " << minPoint.x << ", " << minPoint.y << endl;

	// draw a point at min point:
	//if (minPoint.inside(Rect(0,0,inputImage.cols, inputImage.rows))){
	if (minVal < threshVal){ // if true we suppose that the template was matched:

		if (traceFromPrevPoint){
			int tolVal = templ.cols*0.75; 

			// if prevPoint is not defined : a) first call, b) previous minPoint was erroned, c)
			//	than define as the center of rectangle of detected object:
			if (prevPoint.x == -1 || prevPoint.y == -1){
				Rect *r = buffer->getInternalElement<Rect>(nameOfDetObj);
				if (r== NULL)
					return;
				
				prevPoint.x = (*r).x + 0.5*(*r).width;
				prevPoint.y = (*r).y + 0.5*(*r).height;
				
			}

			// verify is distance between prevPoint and new minPoint is not higher than a tolerance value:
			int dist2 = (minPoint.x - prevPoint.x)*(minPoint.x - prevPoint.x) + (minPoint.y - prevPoint.y)*(minPoint.y - prevPoint.y);
			if (dist2 >= tolVal*tolVal){
				return;
			}else{
				prevPoint = minPoint;
			}
			
		}
		
		// draw a new rectangle:
		Rect rect(minPoint, minPoint + Point(templ.cols, templ.rows));
		rectangle(buffer->getOutputImage("orig"), rect, Scalar(0,255,0));
		
		if (replaceTempl){
			Mat newTempl = inputImage(rect);
			buffer->setInternalImage(templName, newTempl);
		}

	}

	// return:
	buffer->setOutputImages(result, input.getChannelNumber());
	buffer->setInternalElement<Point>("min_point", minPoint);


}

#endif

