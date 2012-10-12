
#ifndef PIPELINEBUFFER_H
#define PIPELINEBUFFER_H


#include <opencv2/opencv.hpp>
using namespace cv;

#include <vector>
#include <map>
#include <string>
using namespace std;


#include "MultiTypeStorage.h"


enum VariableType {CHAR_TYPE, INT_TYPE, FLOAT_TYPE, DOUBLE_TYPE};

class InternalVariable
{
	
	VariableType type;
	char _charv;
	int _intv;
	float _floatv;
	double _doublev;
	

public:
	InternalVariable() : type(INT_TYPE) {_intv=0; _floatv=0.; _doublev=0.0; _charv = ' ';};
	InternalVariable(char c) : type(CHAR_TYPE),  _charv(c) {_intv=0; _floatv=0.; _doublev=0.0;};
	InternalVariable(int i) : type(INT_TYPE),  _intv(i) {_charv=' '; _floatv=0.; _doublev=0.0;};
	InternalVariable(float f) : type(FLOAT_TYPE),  _floatv(f) {_intv=0; _charv=' '; _doublev=0.0;};
	InternalVariable(double d) : type(DOUBLE_TYPE),  _doublev(d) {_intv=0; _floatv=0.; _charv=' ';};

	inline VariableType GetType() const {return type;}
	
	inline char GetCharValue() const {return _charv;}
	inline int GetIntValue() const {return _intv;}
	inline float GetFloatValue() const {return _floatv;}
	inline double GetDoubleValue() const {return _doublev;}


};

class PipelineBuffer
{
	vector<Mat> outputImages;  // [0] is the original image, [1],[2] other outputs
	
	// old containers:
	//map<string,Mat> internalImages;
	//map<string,vector<KeyPoint> > internalKeyPoints;
	//map<string,InternalVariable> internalVariables; 
	
	// new container:
	MultiTypeStorage _storage;

public:
	
	PipelineBuffer(){

	}

	inline Mat & getOutputImage(int i) {return outputImages[i];}
	inline Mat & getOutputImage(string s) {if (s == "orig") return outputImages[0];}


	inline void setInternalImage(string name, Mat & value) { 

	//	map<string,Mat>::iterator it = internalImages.find(name);	
	//	if (it == internalImages.end())
	//		internalImages.insert(pair<string, Mat>(name,value)); 
	//	else
	//		it->second = value;
	
		_storage.setElement<Mat>(name, value);

	} // insert checks if the same key exists, if so does not insert or replace the value. However for Mat that shares the same datacontainer, value modification happens automatically.


	// use this method in callback functions
	inline void setInternalImage(string name, int i){
		if (i<outputImages.size()){
			_storage.setElement<Mat>(name, outputImages[i]);
		}
   }

	//inline void setInternalVariable(string name, InternalVariable & value) { 
		//	map<string, InternalVariable>::iterator it = internalVariables.find(name);	
		//if (it == internalVariables.end())
		//	internalVariables.insert(pair<string, InternalVariable>(name,value)); 
		//else
		//	it->second = value;
	//}	
	template<typename T>
	inline void setInternalVariable(string name, T & value){
		_storage.setElement<T>(name, value);
	}

	

	inline void setInternalKeyPoints(string name, vector<KeyPoint> & kpoints) { 
		//map<string, vector<KeyPoint> >::iterator it = internalKeyPoints.find(name);	
		//if (it == internalKeyPoints.end())
		//	internalKeyPoints.insert(pair<string, vector<KeyPoint> >(name,kpoints)); 
		//else
		//	it->second = kpoints;
		_storage.setElement<vector<KeyPoint> >(name, kpoints);
	}	

	
	

	//// common method to insert any type of variable
	template<typename T>
	inline void setInternalElement(string name, T & t){
		_storage.setElement<T>(name, t);
	}


    inline Mat * getInternalImage(string name) {
		//map<string, Mat>::iterator it = internalImages.find(name);
		//return (it != internalImages.end()) ? it->second : internalImages["NULL"]; 
		return _storage.getElement<Mat>(name);
	}

	template<typename T>
    inline T * getInternalVariable(string name) {
		//map<string, InternalVariable>::iterator it = internalVariables.find(name);
		//return (it != internalVariables.end()) ? it->second : internalVariables["NULL"]; 
		return _storage.getElement<T>(name);
	}

	
    inline vector<KeyPoint> * getInternalKeyPoints(string name) {
		//map<string, vector<KeyPoint> >::iterator it = internalKeyPoints.find(name);	
		//return (it != internalKeyPoints.end()) ? it->second : internalKeyPoints["NULL"]; 
		return _storage.getElement<vector<KeyPoint> >(name); 
	}
	

	//// common method to get any type of variable
	template<typename T>
	inline T * getInternalElement(string name){
		return _storage.getElement<T>(name);
	}




	inline int setOutputImages(Mat & value, int i=-1) {
		if (i==-1 || i >= outputImages.size()){ // no index indicated
			outputImages.push_back(value);
			return outputImages.size()-1;
		}else{
			outputImages[i] = value;
			return i;
		}	
		
	}

	
	// method copies all internal elements (Mat, keypoints, variables) from the current buffer into indicated buffer
	inline void copyBufferTo(PipelineBuffer & buffer){
		// copy InternalImages: 
		//map<string, Mat>::iterator it = internalImages.begin();
		//for (;it!=internalImages.end();it++)
		//	buffer.setInternalImage(it->first, it->second); 

		// copy InternalKeyPoints: 
		//map<string, vector<KeyPoint> >::iterator it2 = internalKeyPoints.begin();
		//for (;it2!=internalKeyPoints.end();it2++)
		//	buffer.setInternalKeyPoints(it2->first, it2->second);

		// copy InternalVariables:
		//map<string, InternalVariable>::iterator it3 = internalVariables.begin();
		//for(;it3!=internalVariables.end();it3++)
		//	buffer.setInternalVariable(it3->first, it3->second);

		_storage.copyStorageTo(buffer._storage);

	}


};

#endif
