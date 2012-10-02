#pragma once 

#include "ImageAbstractFilter.h"


class MathOperationFilter : public ImageAbstractFilter
{
	
public:
	enum OperationType {MULT, SUM, DIFF, OR, AND};
	OperationType type;
	MathOperationFilter(OperationType optype){
		type = optype;
	}
	~MathOperationFilter(){}
	
	inline void ApplyFilter(PipelineInput & input, PipelineBuffer * buffer);
	

};


inline void MathOperationFilter::ApplyFilter(PipelineInput & input, PipelineBuffer * buffer)
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

	}else if (input.getNumberOfInputs() == 3){ // math operation for 3 inputs:
		
		int channel_1 = input.getChannelNumber(0);
		int channel_2 = input.getChannelNumber(1);
		int channel_3 = input.getChannelNumber(2);

		Mat output = buffer->getOutputImage(channel_1);
		Mat input2 = buffer->getOutputImage(channel_2);

		if (type == MULT){
			output = output.mul(input2);
			output = output.mul(buffer->getOutputImage(channel_3));
		}
		else if (type == SUM){
			output += input2; 
			output += buffer->getOutputImage(channel_3);
		}
		else if (type == OR){
			output |= input2;
			output |= buffer->getOutputImage(channel_3);
		}
		else if (type == AND){
			output &= input2;
			output &= buffer->getOutputImage(channel_3);
		}
		
		buffer->setOutputImages(output, channel_1);
	}

}


