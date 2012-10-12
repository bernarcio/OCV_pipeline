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
#include "PipelineFunction.h"


class Pipeline
{
	Mat inputImage;
	
	typedef pair<PipelineInput, ImageAbstractFilter *> PipelineElementType;
	typedef vector<PipelineElementType> PipelineType;
	PipelineType pipelineFilters; // contains all filters and splits
	
	vector<PipelineAbstractFunction *> pipelineFunctions; // functions that may be executed by KeyboardHandler  


	PipelineBuffer * buffer;
	//PipelineBuffer * sharedBuffer; // no need
	int splitsCounter;
	vector<bool> linesToShow;



protected:

	bool bufferIsReplaced;

public:
	// method that deletes this->buffer and assign to another pointer. Used with PipelineSwitch when pipeline1 and pipeline2 write directly into pipeline.buffer instead of their own buffers and then copy them into pipeline.buffer.
	inline void replaceBufferWith(PipelineBuffer * anotherBuffer){
		if (buffer != NULL)
			delete buffer;

		buffer = anotherBuffer;
		// flag to indicate that the buffer should not be deleted when this pipeline is destroyed
		bufferIsReplaced = true;
	} 

public:


	Pipeline(/*PipelineBuffer * sharedbuffer = 0*/){
		pipelineFilters.clear();
		
		buffer =  new PipelineBuffer();
		//sharedBuffer = sharedbuffer;

		splitsCounter = 0;
		bufferIsReplaced = false;
		
		// 0 corresponds to original image and it is always shown:
		linesToShow.push_back(true);
	}

    inline ~Pipeline();

	inline PipelineBuffer * shareBuffer(){return buffer;}
	
	inline void setInputImage(Mat & img) {inputImage = img;	}
    inline void processPipeline();
	inline Mat & getOutputImage(int i=0) {return buffer->getOutputImage(i);}
	

	
	// ----------- addFilter()  ------------------------

	inline void addFilter(PipelineInput input, ImageAbstractFilter *filter) {
		PipelineElementType p(input, filter);	
		pipelineFilters.push_back(p);

		if (input.getChannelNumber()==linesToShow.size())
			linesToShow.push_back(true);
		

	}
	
	inline void addFilter(Pipeline & pipeline){
		// put pipelineFilters into main pipelineFilters: (splits are copied automatically)
		PipelineType::iterator iter = pipeline.pipelineFilters.begin();
		for(;iter!= pipeline.pipelineFilters.end();++iter){
			this->pipelineFilters.push_back(*iter);
		}
		pipeline.pipelineFilters.clear();

	}


	// test : works
	template<typename Filter>
	inline void addFilter(PipelineInput input, Filter * _filter){

		ImageAbstractFilter * filter = dynamic_cast<ImageAbstractFilter *>(_filter);
		
		PipelineElementType p(input, filter);	
		pipelineFilters.push_back(p);

		if (input.getChannelNumber()==linesToShow.size())
			linesToShow.push_back(true);
	}

	// ------------------- addFunction -------------------------------

	
	inline void addFunction(PipelineAbstractFunction * function){
		pipelineFunctions.push_back(function);
	}
	
	// ------------- getBuffer elements -------------------------------
	inline Mat * getBufferImage(string name) {return buffer->getInternalImage(name);}
	inline vector<KeyPoint> * getBufferKeyPoints(string name) {return buffer->getInternalKeyPoints(name);}

	template<typename T> 
	inline T * getBufferElement(string name) {
		return buffer->getInternalElement<T>(name);
	}


	// addSplit : PipelineInput input defines the processing line to copy 
	inline void addSplit(PipelineInput input) {
		ImageAbstractFilter * s = new PipelineSplit();
		PipelineElementType p(input, s);	
		pipelineFilters.push_back(p);
		splitsCounter++;

		if (input.getChannelNumber()==linesToShow.size())
			linesToShow.push_back(true);

	}

	
	// addSwitch: usage when user want to alter pipeline1 and pipeline2 according to the frame number. i.g. execute pipeline1 on 100*n frame (n=0,1,2,3...) and use pipeline2 on other frames.
	// SwitchCondition is a functor with an internal state (numbers of frame to skip for the pipeline1, i.e. 5 -> pipeline1 is used on 0,6,12,17 etc and pipeline2 is executed on 1,2,3,4,5 and 7,8,9,10,11 etc)
	// operator() return 0 or 1 to choose the pipeline (1 or 2)
	template<typename SwitchCondition>
	inline void addSwitch(PipelineInput input, SwitchCondition * condition, Pipeline & pipeline1, Pipeline & pipeline2){
		ImageAbstractFilter * sw = new PipelineSwitch<SwitchCondition>(condition, pipeline1, pipeline2);
		PipelineElementType p(input, sw);	
		pipelineFilters.push_back(p);
		
		if (input.getChannelNumber()==linesToShow.size())
			linesToShow.push_back(true);

		// by default pipeline1 and pipeline2  buffers are shared with this->buffer
		pipeline1.replaceBufferWith(buffer);
		pipeline2.replaceBufferWith(buffer);
	}


	inline int getNumberOfOutputs() const {return linesToShow.size();}


	inline void setLinesToShow(int l1){ 
		if (l1 < linesToShow.size() && l1 > 0){
			// reset all:
			for (int i=1;i<linesToShow.size();i++)
				linesToShow[i] = false;
			// set visible only one
			linesToShow[l1] = true;
		}
	};

	inline void setLinesToShow(int l1, int l2){
		if (l1 < linesToShow.size() && l1 > 0 && l2 < linesToShow.size() && l2 > 0 && l1 != l2){
			// reset all:
			for (int i=1;i<linesToShow.size();i++)
				linesToShow[i] = false;
			// set visible only one
			linesToShow[l1] = true;
			linesToShow[l2] = true;
		}
	};

	inline void setLinesToShow(int l1, int l2, int l3){
		if (l1 < linesToShow.size() && l1 > 0 && l2 < linesToShow.size() && l2 > 0 && l1 != l2 && l3 < linesToShow.size() && l3 > 0 && l3 != l2 && l3 != l1){
			// reset all:
			for (int i=1;i<linesToShow.size();i++)
				linesToShow[i] = false;
			// set visible only one
			linesToShow[l1] = true;
			linesToShow[l2] = true;
			linesToShow[l3] = true;
		}
	}
	
	//			 0		1	  2		 3		
	// lines = {true, true, false, false}
	inline void setLinesToShow(vector<bool> & lines){
		//...
	};

	inline bool isLineShown(int i){
		return linesToShow[i];
	}
};




void Pipeline::processPipeline()
{
	Mat inputImageCopy;
	inputImage.copyTo(inputImageCopy);
	
	// Set camera's image into the placement 0 of the buffer
	buffer->setOutputImages(inputImage, 0);
	// Set the copy into the placement 1 of the buffer
	if (linesToShow.size()!=1)
		buffer->setOutputImages(inputImageCopy, 1);

	PipelineType::iterator iter = pipelineFilters.begin();
	for (;iter!=pipelineFilters.end(); ++iter){
		(*iter).second->ApplyFilter((*iter).first, buffer);
	}

	// execute callback functions:
	vector<PipelineAbstractFunction *>::iterator iter2 = pipelineFunctions.begin();
	for(;iter2!=pipelineFunctions.end();iter2++){
		if ((*iter2)->getFunctionType()==PipelineAbstractFunction::CALLED_AFTER_FILTERS)
			(*iter2)->executeFunction();	
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
		if (p.second->deleted == false){ // does not work for two pipelines with one filter : second pipeline's filter adress does not become NULL
			delete p.second;
		}
        pipelineFilters.pop_back();
        s--;
    }
	
	// delete buffer:
	if (buffer != NULL && bufferIsReplaced == false){
		delete buffer;
		buffer = NULL;
	}

	// delete pipelineFunctions:
	int s2 = pipelineFunctions.size();
	while (s2 > 0){
		if (pipelineFunctions[s2-1] != NULL)
			delete pipelineFunctions[s2-1];

		pipelineFunctions.pop_back();
		s2--;
	}

}

#endif // PIPELINE_H
