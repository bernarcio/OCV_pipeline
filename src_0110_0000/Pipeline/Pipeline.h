/**
	\class Pipeline
	\brief Class is used to model the image processsing pipeline with filters, splits, sums etc

	PipelineBuffer stores : output images, internal images (if not only one image is passed between two filters), internal variables (e.g. parameters coming from computing the histogram)


	Graphical repersentation looks like:


	PipelineBuffer: 0 (CamInput), 1 (1st processing line), 2 (2nd processing line), etc
	
	 Initialization 
	0 ------.-------||--------------------------------------------------------------------------------------- Camera Input Image
			|       ||								   
	1		L-------||--(Filter_A)---*---(Filter_B)----*----------------(MULT)-------.-----------------(SUM)---  
			        ||			     |				   |           	      |	         |				     |
	2		        ||			     L---(Filter_C)-*--E-----(Filter_D)---.-------(SUM)----(Filter_G)---.-----
			        ||							    |  |  				    	     |
	3		        ||							    L--E-------(Filter_E)------------.-------------------------
					||								   |	
	4				||								   L--------(Filter_F)-------------------------------------	
	
	*  - class SplitFilter : public ImageAbstractFilter
	(Filter_X)  - class AnImageFilter : public ImageAbstractFilter
	(X) - class MathOpFilter : public ImageAbstractFilter

	Pipeline::addFilter(PipelineInput input, ImageAbstractFilter * filter) 
	{
		pipeline.push_back(pair<input, filter>)
	}
	"input" specifies the lines of the pipelineBuffer to use


	in this implementation pipeline execution is linear:
	filter is takes care of the geometry of the pipeline

	Pipeline::processingPipeline(){
		iter = pipeline.begin()
		for(;iter!=pipeline.end();++iter){
			(*iter)->ApplyFilter(&pipelineBuffer, input)
		}
	}


*/

#ifndef PIPELINE_H
#define PIPELINE_H

#include "ImageAbstractFilter.h"
#include "PipelineBuffer.h"
#include "PipelineInput.h"
#include "PipelineSplit.h"
#include "PipelineSwitch.h"



class Pipeline
{
	Mat inputImage;
	typedef pair<PipelineInput, ImageAbstractFilter *> PipelineElementType;
	typedef vector<PipelineElementType> PipelineType;
	PipelineType pipelineFilters; // contains all filters and splits
	 	
	PipelineBuffer * buffer;
	PipelineBuffer * sharedBuffer;

	int splitsCounter;

public:
	Pipeline(PipelineBuffer * sharedbuffer = 0){
		pipelineFilters.clear();
		
		buffer =  new PipelineBuffer();
		sharedBuffer = sharedbuffer;

		splitsCounter = 0;
		
	}

    inline ~Pipeline();

	inline PipelineBuffer * shareBuffer(){return buffer;}

	inline void setInputImage(Mat & img) {inputImage = img;	}
    inline void processPipeline();
	inline Mat & getOutputImage(int i=0) {return buffer->getOutputImage(i);}
	

	
	inline void addFilter(PipelineInput input=1, ImageAbstractFilter *filter=NULL) {
		PipelineElementType p(input, filter);	
		pipelineFilters.push_back(p);
	}

	inline void addFilter(Pipeline & pipeline){
		// put pipelineFilters into main pipelineFilters: (splits are copied automatically)
		PipelineType::iterator iter = pipeline.pipelineFilters.begin();
		for(;iter!= pipeline.pipelineFilters.end();++iter){
			this->pipelineFilters.push_back(*iter);
		}
		pipeline.pipelineFilters.clear();

	}


	inline Mat & getBufferImage(string name) {return buffer->getInternalImage(name);}


	// addSplit : PipelineInput input defines the processing line to copy 
	inline void addSplit(PipelineInput input) {
		ImageAbstractFilter * s = new PipelineSplit();
		PipelineElementType p(input, s);	
		pipelineFilters.push_back(p);
		splitsCounter++;
	}

	
	// addSwitch: usage when user want to alter pipeline1 and pipeline2 according to the frame number. i.g. execute pipeline1 on 100*n frame (n=0,1,2,3...) and use pipeline2 on other frames.
	// SwitchCondition is a functor with an internal state (numbers of frame to skip for the pipeline1, i.e. 5 -> pipeline1 is used on 0,6,12,17 etc and pipeline2 is executed on 1,2,3,4,5 and 7,8,9,10,11 etc)
	// operator() return 0 or 1 to choose the pipeline (1 or 2)
	template<typename SwitchCondition>
	inline void addSwitch(PipelineInput input, SwitchCondition * condition, Pipeline & pipeline1, Pipeline & pipeline2){
		ImageAbstractFilter * sw = new PipelineSwitch<SwitchCondition>(condition, pipeline1, pipeline2);
		PipelineElementType p(input, sw);	
		pipelineFilters.push_back(p);
	}



	inline int getNumberOfOutputs() const {return splitsCounter + 2;}
};




void Pipeline::processPipeline()
{
	Mat inputImageCopy;
	inputImage.copyTo(inputImageCopy);
	
	// Set camera's image into the placement 0 of the buffer
	buffer->setOutputImages(inputImage, 0);
	// Set the copy into the placement 1 of the buffer
	buffer->setOutputImages(inputImageCopy, 1);

	PipelineType::iterator iter = pipelineFilters.begin();
	for (;iter!=pipelineFilters.end(); ++iter){
		(*iter).second->ApplyFilter((*iter).first, buffer);
	}

}




Pipeline::~Pipeline()
{	
 //   // delete splits:
	//int s = pipelineSplits.size();
	//while (s > 0){
	//	delete pipelineSplits[s-1];
	//	pipelineSplits.pop_back();
	//	s--;
	//}


    // delete filters and splits:
    int s = pipelineFilters.size();
    while(s > 0){
        PipelineElementType p = pipelineFilters[s-1];
        delete p.second;
        pipelineFilters.pop_back();
        s--;
    }
	
	// delete buffer:
	delete buffer;

}

#endif // PIPELINE_H
