#ifndef PIPELINESWITCH_H
#define PIPELINESWITCH_H

#include "ImageAbstractFilter.h"

#include "Pipeline.h"

class Pipeline;



class SwitchFunctor
{
	int numberOfFramesToSkip;
public:
	SwitchFunctor(int nfs=10){
		numberOfFramesToSkip = nfs;
	}

	inline int operator()(int currFrame){
		int c = currFrame % (numberOfFramesToSkip+1);
		return (c == 0) ? 0 : 1;
	}

};



template<class SwitchCondition>
class PipelineSwitch : public ImageAbstractFilter
{

	vector<Pipeline *> pipeline;
	SwitchCondition * switcher;
	int frameNumber;

public:
	
	
	PipelineSwitch(SwitchCondition * cond, Pipeline & pipeline1, Pipeline & pipeline2)
	{
		switcher = cond;
		pipeline.push_back(&pipeline1);	
		pipeline.push_back(&pipeline2);
		frameNumber = 0;
	};


	~PipelineSwitch(){};
	
	
	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);

};


template<typename SwitchCondition>
inline void PipelineSwitch<SwitchCondition>::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	Mat img = buffer->getOutputImage(input.getChannelNumber());

	int pipelineNumber = switcher->operator()(frameNumber);
	pipeline[pipelineNumber]->setInputImage(img);
	pipeline[pipelineNumber]->processPipeline();


	frameNumber++;

	// return :
	Mat outputOrig = pipeline[pipelineNumber]->getOutputImage();
	// get last channel line:
	int channel = pipeline[pipelineNumber]->getNumberOfOutputs()-1;
	Mat output1 = pipeline[pipelineNumber]->getOutputImage(channel);
	buffer->setOutputImages(outputOrig, 0);
	buffer->setOutputImages(output1, input.getChannelNumber());

}


#endif