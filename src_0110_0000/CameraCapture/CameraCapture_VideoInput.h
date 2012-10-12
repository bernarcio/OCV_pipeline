
#ifndef CAMERACAPTURE_VIDEOINPUT_H
#define CAMERACAPTURE_VIDEOINPUT_H



#include <opencv2/opencv.hpp>
#include "Pipeline.h"

// use special library for video input configurations
#include "videoInput.h"

// keyboard special handling:
#include "KeyboardHandle.h"

using namespace cv;
using namespace std;


class CameraCapture_VideoInput
{
	videoInput VI;
	int device;
	int width, height;

	int deviceR;
	int widthR, heightR;
	int numDevices;

	bool stereo;
	Pipeline * pipeline;
	Pipeline * pipelineR; // for stereo

	KeyboardHandle * keyHandler;
	VideoWriter vwriter;

	


public:
	CameraCapture_VideoInput(Pipeline * p=0, bool stereo=false){
		pipeline = p;
		pipelineR = 0;
		keyHandler = 0;
		// list of opened devices
		device = 0;
		deviceR = 1;
		numDevices = videoInput::listDevices();	
		VI.setUseCallback(false);
		this->stereo=stereo;

		
	}
	~CameraCapture_VideoInput(){
		//Shut down devices properly
		if (stereo){
			VI.stopDevice(device);
			VI.stopDevice(deviceR);
		}
		else
			VI.stopDevice(device);
	}

    inline void setPipeline(Pipeline * p, Pipeline * pr = 0) {pipeline = p; pipelineR = pr;}
	inline void setKeyboardHandler(KeyboardHandle * kbh) {keyHandler = kbh;}
    inline void run(); // Method to start acquisition with pipeline processing
    
	
	//inline void runWithLearning(); // Method to start acquisition, keyboard handling to take pictures and apply pipeline filters



protected:
    inline bool cameraInit();
	inline void keysInterpretation(int c);
};


bool CameraCapture_VideoInput::cameraInit()
{
	
	if (!VI.setupDevice(device, VI_COMPOSITE)){
			cerr << "Camera load error" <<endl;
			return false;
	}

	if (stereo){
		if (!VI.setupDevice(deviceR, VI_COMPOSITE)){
			cerr << "Camera right load error" <<endl; 
			return false;
		}
	}



	width = VI.getWidth(device);
	height = VI.getHeight(device);
	
	if (stereo){
		widthR = VI.getWidth(deviceR);
		heightR = VI.getHeight(deviceR);
	}
	//namedWindow("Acquisition");

	cout << "Camera acquisition is started. Press 'q' to quit. " << endl;
	cout << "Press 's' for camera settings configuration" << endl;

	return true;

}

void CameraCapture_VideoInput::run()
{
	if (!cameraInit()){
		return;
	}

	while(1){
	
		Mat img(height, width, CV_8UC3), imgR;
		if(VI.isFrameNew(device)){
			VI.getPixels(device, img.data, false, true);	//fills pixels as a BGR (for openCV) unsigned char array - no flipping
		}
	
		if (stereo){
			imgR = Mat(heightR, widthR, CV_8UC3);
			if(VI.isFrameNew(deviceR)){
				VI.getPixels(deviceR, imgR.data, false, true);	//fills pixels as a BGR (for openCV) unsigned char array - no flipping
			}	
		}


		// Display:
		// mirror transformation:
		// flip(img,img,1);
		// cvShowImage("Acquisition", &img.operator CvMat());
		

		// Pipeline image processing:
		pipeline->setInputImage(img);
		pipeline->processPipeline();

		// Show pipeline lines:
		for (int i=0; i<pipeline->getNumberOfOutputs();i++){
			if (pipeline->isLineShown(i)){
				Mat pipelineOutImage(pipeline->getOutputImage(i));		
				stringstream ss;
				ss << i;
				string wname = "Pipeline_" + ss.str();
				cvShowImage(wname.c_str(), &pipelineOutImage.operator CvMat());
			}
		}

		

		if (stereo){
			pipelineR->setInputImage(imgR);		
			pipelineR->processPipeline();

		
			for (int i=0; i<pipelineR->getNumberOfOutputs();i++){
				Mat pipelineOutImageR(pipelineR->getOutputImage(i));		
				stringstream ss;
				ss << i;
				string wname = "PipelineR_" + ss.str();
				cvShowImage(wname.c_str(), &pipelineOutImageR.operator CvMat());
			}

		}
		
		// keyboard handling : 
		int c = cvWaitKey(5);
		//int c = cvWaitKey(0);
		if( (char) c == 'q' || (char) c == 27 ) { break; } 
		
		if( (char) c == 's' ) { 
			VI.showSettingsWindow(device);
			if (stereo){
				VI.showSettingsWindow(deviceR);
			}			
		} 
		
		keysInterpretation(c);		
		
	}


}


inline void CameraCapture_VideoInput::keysInterpretation(int c)
{	

	if (keyHandler!=NULL){

			vector<KeyboardHandle::KeyDescription>::iterator it = keyHandler->getKeys().begin();
			for (;it!=keyHandler->getKeys().end();it++){
				if ((*it).key == (char ) c){
					
					if ((*it).action == KeyboardHandle::SAVE_IMAGE_ON_DISK){
						int index = (*it).pipelineChannel; 
						Mat img, *im;
						if (index != -1){
							img = pipeline->getOutputImage(index);
						}else{
							im = pipeline->getBufferImage((*it).bufferElementName);
							if(im !=NULL){
								img = *im;
							}else{
								cerr << "KeyboardHandling: buffer matrix was not found at name : " << (*it).bufferElementName << endl;
								continue;
							}
						}
						imwrite((*it).name, img);
						cout << "Image is saved on the disk" << endl;

					}else if ((*it).action == KeyboardHandle::SAVE_MATRIXDATA_ON_DISK){
						int index = (*it).pipelineChannel;
						Mat img, *im;
						if (index != -1){
							img = pipeline->getOutputImage(index);
						}else{
							im = pipeline->getBufferImage((*it).bufferElementName);
							if(im !=NULL){
								img = *im;
							}else{
								cerr << "KeyboardHandling: buffer matrix was not found at name : " << (*it).bufferElementName << endl;
								continue;
							}
						}
						FileStorage fs((*it).name, FileStorage::WRITE);
						int n = (*it).name.size(); 
						int pos = (*it).name.find_last_of("//");
						string item = (*it).name.substr(pos+1, n-4 - (pos+1));
						fs << item << img;
						fs.release();

						cerr << "Matrix has been saved on the disk" << endl;

					}else if ((*it).action == KeyboardHandle::SAVE_KEYPOINTS_ON_DISK){
						
						vector<KeyPoint> keypoints, *kp = pipeline->getBufferKeyPoints((*it).bufferElementName);
						if (kp != NULL)
							keypoints = *kp;
						else{
							cerr << "KeyboardHandling: Keypoints were not found at name : " << (*it).bufferElementName << endl;
							continue;
						}
						FileStorage fs((*it).name, FileStorage::WRITE);
						int n = (*it).name.size(); 
						int pos = (*it).name.find_last_of("//");
						string item = (*it).name.substr(pos+1, n-4 - (pos+1));
						int s = keypoints.size();
						fs << item << s;
						for (int i=0;i<s;i++){
							stringstream ss;
							ss << "keypoint_" << i;
							fs << ss.str() << "{";
							fs << "x" << keypoints[i].pt.x << "y" << keypoints[i].pt.y;
							fs << "size" << keypoints[i].size;
							fs << "angle" << keypoints[i].angle;
							fs << "response" << keypoints[i].response;
							fs << "octave" << keypoints[i].octave;
							fs << "class_id" << keypoints[i].class_id;
							fs << "}";
						}
						//KeyPoint kp(x,y,size,angle,response,octave,class_id)

						fs.release();

						cerr << "Keypoints have been saved on the disk" << endl;
					
					}else if ((*it).action == KeyboardHandle::SAVE_IMAGE_IN_BUFFER || (*it).action == KeyboardHandle::SAVE_MATRIXDATA_IN_BUFFER){
						int index = (*it).pipelineChannel;
						if (index != -1){
							Mat img = pipeline->getOutputImage(index);
							pipeline->shareBuffer()->setInternalImage((*it).name, img);
							cerr << "Matrix or Image has been saved in the buffer" << endl;
						}else{
							cerr << "pipeline channel error: pipelineChannel = -1" << endl; 
						}
						
					
					}else if ((*it).action == KeyboardHandle::LOAD_MATRIXDATA_FROM_DISK_INTO_BUFFER){
						FileStorage fs((*it).name, FileStorage::READ);
						if (fs.isOpened()){
							Mat img;
							int pos = (*it).name.find_last_of("//");
							int n = (*it).name.size();
							string item = (*it).name.substr(pos+1, n-4 - (pos+1));
							//cout << "item name : " << item << endl;
							fs[item] >> img;
							pipeline->shareBuffer()->setInternalImage((*it).bufferElementName, img);
							cout << "File has been succesfully read and matrix has been loaded into the buffer" << endl;

							imshow("Loaded matrix",img);

						}else{
							cerr << "Can not read yml data file" << endl;
						}
						fs.release();

					}else if ((*it).action == KeyboardHandle::START_REC_VIDEO_ON_DISK ){
						int fourcc = CV_FOURCC('M','J','P','G');
						keyHandler->recPplIndex = (*it).pipelineChannel;
						if (keyHandler->recPplIndex != -1){
							Mat img = pipeline->getOutputImage(keyHandler->recPplIndex);
							vwriter.open((*it).name, fourcc, 15, Size(img.cols, img.rows));
							if(!vwriter.isOpened()){
								cerr << "Cannot open VideoWriter and can not record video." << endl;
							}else{	
								keyHandler->rec = true;						
								cout << "Recording is started" << endl;
							}

						
						}else{
							cerr << "pipeline channel error: pipelineChannel = -1" << endl; 
						}
										
					}else if ((*it).action == KeyboardHandle::STOP_REC_VIDEO_ON_DISK){
						
						if (keyHandler->rec){
							keyHandler->rec = false;
							cout << "Recording is stoped" << endl;
							
						}
					
					}



				} // if ((*it).key == (char ) c){

			} // for (;it!=keyHandler->getKeys().end();it++){


			// if record button was 
			if (keyHandler->rec){
				Mat img = pipeline->getOutputImage(keyHandler->recPplIndex);
				vwriter << img;
			}

	} // if (keyHandler != NULL)


}



/*
void CameraCapture_VideoInput::runWithLearning()
{
	cameraInit();


	cerr << "Acquisition with LearningKit: " << endl;
	cerr << "	Press 'x' key to capture the current image and process with the pipeline" << endl;

    bool imageIsTaken = false;
	
	while(1){
	
		Mat img(height, width, CV_8UC3), imgR;
		if(VI.isFrameNew(device)){
			VI.getPixels(device, img.data, false, true);	//fills pixels as a BGR (for openCV) unsigned char array - no flipping
		}
	
		if (stereo){
			imgR = Mat(heightR, widthR, CV_8UC3);
			if(VI.isFrameNew(deviceR)){
				VI.getPixels(deviceR, imgR.data, false, true);	//fills pixels as a BGR (for openCV) unsigned char array - no flipping
			}	
		}


		// Display:
		// mirror transformation:
		// flip(img,img,1);
		// cvShowImage("Acquisition", &img.operator CvMat());
		

		// Display original images:
		string wname = "Original image";
		cvShowImage(wname.c_str(), &img.operator CvMat());
		if (stereo){
			string wname = "Original image R";
			cvShowImage(wname.c_str(), &imgR.operator CvMat());
		}




		// keyboard handling :
		int c = cvWaitKey(5);
		if( (char) c == 'q' || (char) c == 27 ) { break; } 
		
		if( (char) c == 's' ) { 
			VI.showSettingsWindow(device);
			if (stereo){
				VI.showSettingsWindow(deviceR);
			}			
		} 


		if ((char) c == 'x'){
			// take a photo and process it
            imageIsTaken = true;


            // Pipeline image processing:
            pipeline->setInputImage(img);
            pipeline->processPipeline();

            for (int i=0; i<pipeline->getNumberOfOutputs();i++){
                Mat pipelineOutImage(pipeline->getOutputImage(i));
                stringstream ss;
                ss << i;
                string wname = "Pipeline_" + ss.str();
                cvShowImage(wname.c_str(), &pipelineOutImage.operator CvMat());
            }


            if (stereo){
                pipelineR->setInputImage(imgR);
                pipelineR->processPipeline();


                for (int i=0; i<pipelineR->getNumberOfOutputs();i++){
                    Mat pipelineOutImageR(pipelineR->getOutputImage(i));
                    stringstream ss;
                    ss << i;
                    string wname = "PipelineR_" + ss.str();
                    cvShowImage(wname.c_str(), &pipelineOutImageR.operator CvMat());
                }

            }

		
		}




		
	}


    // close all CV windows:

    cvDestroyWindow("Original image");
    if (stereo)
        cvDestroyWindow("Original image R");

    if (imageIsTaken){
        for (int i=0; i<pipeline->getNumberOfOutputs();i++){
            stringstream ss;
            ss << i;
            string wname = "Pipeline_" + ss.str();
            cvDestroyWindow(wname.c_str());
        }

        if (stereo){
            for (int i=0; i<pipelineR->getNumberOfOutputs();i++){
                stringstream ss;
                ss << i;
                string wname = "PipelineR_" + ss.str();
                cvDestroyWindow(wname.c_str());
            }
        }
    }


}
*/

#endif // CAMERACAPTURE_VIDEOINPUT_H
