#ifndef MATCHTEMPLATEFILTER_H
#define MATCHTEMPLATEFILTER_H

#include "ImageAbstractFilter.h"

class MatchTemplateFilter : public ImageAbstractFilter
{


public:
	enum FilterType {SQDIFF, SQDIFF_NORMED, CCORR, CCORR_NORMED, CCOEFF, CCOEFF_NORMED};
	MatchTemplateFilter(FilterType t=SQDIFF, string tName="templ"){
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
	}

	~MatchTemplateFilter(){};
	

	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

private:
	FilterType type;
	string templName; // name of the template to search Matrix in the pipeline buffer, by default "templ"
	int method;

};



inline void MatchTemplateFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	Mat result, templ;
	templ = buffer->getInternalImage(templName);
	if (templ.data == NULL)
		return;
	else{
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

	buffer->setOutputImages(result, input.getChannelNumber());

}

#endif

