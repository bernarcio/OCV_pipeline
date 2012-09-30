#ifndef KEYBOARDHANDLE_H
#define KEYBOARDHANDLE_H

#include <iostream>
using namespace std;




class KeyboardHandle
{

	
public:
	enum StdAction {SAVE_IMAGE_ON_DISK, SAVE_MATRIXDATA_ON_DISK, LOAD_MATRIXDATA_FROM_DISK_INTO_BUFFER, SAVE_IMAGE_IN_BUFFER, SAVE_MATRIXDATA_IN_BUFFER, EXEC_FUNCTION};
	struct KeyDescription
	{
		char key;
		StdAction action;
		int pipelineChannel;
		string bufferElementName;
		string name;	

		void (*function)(vector<Mat> &);
		
		KeyDescription(char k, StdAction ac, int pc, string nm):
			key(k), action(ac), pipelineChannel(pc), name(nm) 
			{
				bufferElementName = "";
			}	
		KeyDescription(char k, StdAction ac, string be, string nm):
			key(k), action(ac), bufferElementName(be), name(nm) 
			{
				pipelineChannel = -1;
			}	

		KeyDescription(char k, void (* f)(vector<Mat> &), string be) : key(k) {
			action = EXEC_FUNCTION;
			pipelineChannel = -1;
			function = f;
			bufferElementName=be;
			name = "";
		}


	};

//	KeyboardHandle(){};
//	~KeyboardHandle(){};

	inline void addKey(char key, StdAction a=SAVE_IMAGE_ON_DISK, int pipelineChannel=0, string name="image.jpg");
	inline void addKey(char key, StdAction a, string buffElemName, string name);
	inline vector<KeyDescription> & getKeys() {return keys;}
	

private:
	vector<KeyDescription> keys; // allows overloading of one key. previously used a map<char, KeyDescription>
};



inline void KeyboardHandle::addKey(char key, StdAction a, int pipelineChannel, string name)
{
	KeyDescription kd(key, a, pipelineChannel, name);
	keys.push_back(kd);
}

inline void KeyboardHandle::addKey(char key, StdAction a, string be, string name)
{
	KeyDescription kd(key, a, be, name);
	keys.push_back(kd);
}



#endif