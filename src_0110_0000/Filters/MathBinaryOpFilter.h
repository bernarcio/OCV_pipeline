#pragma once 

#include "ImageAbstractFilter.h"


class MathBinaryOpFilter : public ImageAbstractFilter
{
	
public:
	enum OperationType {MULT, SUM, DIFF, OR, AND};
	OperationType type;
	MathBinaryOpFilter(OperationType optype){
		type = optype;
	}
	~MathBinaryOpFilter(){}
	
	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);
	

};


inline void MathBinaryOpFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
{
	if (input.getNumberOfInputs() == 2){ // binary math operation
		int channel_1 = input.getChannelNumber(0);
		int channel_2 = input.getChannelNumber(1);
	
		Mat output = buffer->getOutputImage(channel_1);

		if (type == MULT)
			output = output.mul(buffer->getOutputImage(channel_2));
		else if (type == SUM)
			output += buffer->getOutputImage(channel_2);
		else if (type == DIFF)
			output -=  buffer->getOutputImage(channel_2);
		else if (type == OR)
			output |= buffer->getOutputImage(channel_2);
		else if (type == AND)
			output &= buffer->getOutputImage(channel_2);
	
		buffer->setOutputImages(output, channel_1);

	}

}


