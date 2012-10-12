
#ifndef IMAGEABSTRACTFILTER_H
#define IMAGEABSTRACTFILTER_H

#include "PipelineBuffer.h"
#include "PipelineInput.h"


class ImageAbstractFilter
{

	
public:
	// test: works
	bool deleted;

	inline ImageAbstractFilter() : deleted(false) {};
	virtual ~ImageAbstractFilter(){ deleted = true; };
	virtual void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer) = 0; 

};


#endif
