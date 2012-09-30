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



class Pipeline
{
	Mat inputImage;
	typedef pair<PipelineInput, ImageAbstractFilter *> PipelineElementType;
	typedef vector<PipelineElementType> PipelineType;
	PipelineType pipeline;
	 	
	PipelineBuffer * buffer;
	PipelineBuffer * sharedBuffer;
	
	vector<ImageAbstractFilter*> pipelineSplits; // we keep splits in the pipeline that user is not obliged to create it explicitly before adding


public:
	Pipeline(PipelineBuffer * sharedbuffer = 0){
		pipeline.clear();
		
		buffer =  new PipelineBuffer();
		sharedBuffer = sharedbuffer;
		
	}

    inline ~Pipeline();

	inline PipelineBuffer * shareBuffer(){return buffer;}

	inline void setInputImage(Mat & img) {inputImage = img;	}
    inline void processPipeline();
	inline Mat & getOutputImage(int i=0) {return buffer->getOutputImage(i);}
	inline void addFilter(PipelineInput input=1, ImageAbstractFilter *filter=NULL) {
		PipelineElementType p(input, filter);	
		pipeline.push_back(p);
	}

	inline Mat & getBufferImage(string name) {return buffer->getInternalImage(name);}

	// addSplit : PipelineInput input defines the processing line to copy 
	inline void addSplit(PipelineInput input) {
		ImageAbstractFilter * s = new PipelineSplit();
		pipelineSplits.push_back(s);
		PipelineElementType p(input, s);	
		pipeline.push_back(p);
	}

	inline int getNumberOfOutputs() const {return pipelineSplits.size()+2;}
};




void Pipeline::processPipeline()
{
	Mat inputImageCopy;
	inputImage.copyTo(inputImageCopy);
	
	// Set camera's image into the placement 0 of the buffer
	buffer->setOutputImages(inputImage, 0);
	// Set the copy into the placement 1 of the buffer
	buffer->setOutputImages(inputImageCopy, 1);

	PipelineType::iterator iter = pipeline.begin();
	for (;iter!=pipeline.end(); ++iter){
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
    int s = pipeline.size();
    while(s > 0){
        PipelineElementType p = pipeline[s-1];
        delete p.second;
        pipeline.pop_back();
        s--;
    }
	
	// delete buffer:
	delete buffer;

}

#endif // PIPELINE_H
