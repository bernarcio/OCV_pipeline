#include "qcameracapture.h"



QCameraCapture::QCameraCapture(QObject *parent, Pipeline *p, bool stereo) :
    QThread(parent)
{

    pipeline = p;
    pipelineR = 0;

    // list of opened devices (a priori)
    device = 0;
    deviceR = 1;
    this->stereo=stereo;

    runningStatus = false;

    takeShot = false;

}


QCameraCapture::~QCameraCapture()
{

    cout<< "QCameraCapture::~QCameraCapture()" << endl;
    stop();

}



void QCameraCapture::cameraInit()
{
    cout<< "void QCameraCapture::cameraInit()" << endl;

    if (!VI->setupDevice(device)){
        cout<< "Camera load error" <<endl;
        return;
    }

    if (stereo){
        if (!VI->setupDevice(deviceR)){
            cout<< "Camera right load error" <<endl;
            return;
        }
    }



    width = VI->getWidth(device);
    height = VI->getHeight(device);

    if (stereo){
        widthR = VI->getWidth(deviceR);
        heightR = VI->getHeight(deviceR);
    }

    //cout << "Camera acquisition is started. Press 'q' to quit. " << endl;
    cout << "Press 's' for camera settings configuration" << endl;
    //cout << "w,h : " << width << ", " << height << endl;

    runningStatus=true;

}



void QCameraCapture::startAcquisition()
{

    //cout<< "void QCameraCapture::startAcquisition()" << endl;

    Mat img(height, width, CV_8UC3), imgR;
    //cout << "w,h : " << width << ", " << height << endl;

    if(VI->isFrameNew(device)){
        VI->getPixels(device, img.data, false, true);	//fills pixels as a BGR (for openCV) unsigned char array - no flipping
    }

    if (stereo){
        imgR = Mat(heightR, widthR, CV_8UC3);
        if(VI->isFrameNew(deviceR)){
            VI->getPixels(deviceR, imgR.data, false, true);	//fills pixels as a BGR (for openCV) unsigned char array - no flipping
        }
    }


    // Display:
    // mirror transformation:
    // flip(img,img,1);
    //cvShowImage("Acquisition", &img.operator CvMat());


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

    // keyboard handling :
    int c = cvWaitKey(5);

    if( (char) c == 's' ) {
        VI->showSettingsWindow(device);
        if (stereo){
            VI->showSettingsWindow(deviceR);
        }
    }


    if (takeShot){
        cout << "takeShot = true" << endl;
        waitCondition.wait(&mutex);

        // When wakeAll is called -> mutex is locked again. See QWaitCondition docs
        //cout << "after lock()" << endl;
        takeShot = false;
        //cout << "takeShot = false" << endl;
    }




}


void QCameraCapture::Continue()
{
    waitCondition.wakeAll();

}




void QCameraCapture::run()
{
    cout<< "void QCameraCapture::run()" << endl;

    VI = new videoInput();
    numDevices = videoInput::listDevices();
    VI->setUseCallback(false);

    cameraInit();


    // lock mutex:
    mutex.lock();

    while(runningStatus){

        startAcquisition();

    }

    mutex.unlock();



    // delete VI:
    //Shut down devices properly
    if (stereo){
        VI->stopDevice(device);
        VI->stopDevice(deviceR);
    }
    else
        VI->stopDevice(device);

    cout<< "before : emit SendOnFinished(0);" << endl;
    emit SendOnFinished(0);

}

