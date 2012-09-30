#ifndef PIPELINESPLIT_H
#define PIPELINESPLIT_H


#include "ImageAbstractFilter.h"


class PipelineSplit : public ImageAbstractFilter
{
	bool firstCall;
	int processingLine;
	

public:
	PipelineSplit(){
		firstCall = true;
	};
	~PipelineSplit(){};
	
    inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);
	
};


void PipelineSplit::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	Mat newChannel;
	buffer->getOutputImage(input.getChannelNumber()).copyTo(newChannel);
	if (firstCall){
		processingLine = buffer->setOutputImages(newChannel);
		firstCall=false;
	}else{
		buffer->setOutputImages(newChannel, processingLine);
	}
}

#endif
