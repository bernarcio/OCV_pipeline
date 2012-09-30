
#ifndef IMAGEABSTRACTFILTER_H
#define IMAGEABSTRACTFILTER_H

#include "PipelineBuffer.h"
#include "PipelineInput.h"


class ImageAbstractFilter
{

public:

	virtual ~ImageAbstractFilter(){};
	virtual void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer) = 0; 

};


#endif
