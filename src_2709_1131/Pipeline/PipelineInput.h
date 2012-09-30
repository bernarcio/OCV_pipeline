#ifndef PIPELINEINPUT_H
#define PIPELINEINPUT_H



#include <vector>
using namespace std;

class PipelineInput
{
	int numberOfInputs;
	vector<int> inputs;
public:
	PipelineInput(int i1) : numberOfInputs(1) {inputs.push_back(i1);}
	PipelineInput(int i1, int i2) : numberOfInputs(2) {inputs.push_back(i1); inputs.push_back(i2);}
	PipelineInput(int i1, int i2, int i3) : numberOfInputs(3) {inputs.push_back(i1); inputs.push_back(i2); inputs.push_back(i3);}
	
	PipelineInput & operator= (int i) {
		this->inputs.push_back(i);
		return *this;
	}
	
	inline int getChannelNumber(int i=0) const {return inputs[i];}
	inline int getNumberOfInputs() const {return numberOfInputs;}

};



#endif
