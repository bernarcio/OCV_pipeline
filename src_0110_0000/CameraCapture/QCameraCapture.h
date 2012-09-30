#ifndef QCAMERACAPTURE_H
#define QCAMERACAPTURE_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QHash>
#include <QString>

#include <opencv2/opencv.hpp>
#include "Pipeline.h"

#include "LearnObject.h"

// use special library for video input configurations
#include "videoInput.h"

using namespace cv;
using namespace std;


class QCameraCapture : public QThread
{
    Q_OBJECT

    videoInput * VI;
    int device;
    int width, height;

    int deviceR;
    int widthR, heightR;
    int numDevices;

    bool stereo;
    Pipeline * pipeline;
    Pipeline * pipelineR; // for stereo

    // Photographer:
    bool takeShot;


    // Thread handling variables:
    bool runningStatus;
    QMutex mutex;
    QWaitCondition waitCondition;


public:
    explicit QCameraCapture(QObject *parent = 0, Pipeline * p=0, bool stereo=false);
    
    ~QCameraCapture();


    inline void setPipeline(Pipeline * p, Pipeline * pr = 0) {pipeline = p; pipelineR = pr;}

    void stop() {runningStatus = false;}

    void TakeShot(){takeShot=true;}
    void Continue();

protected:
    void run();
    void cameraInit();
    void startAcquisition();

signals:
    void SendOnFinished(int); //!< signal emitted when thread running is stoped : 0 - ok, 1 - exited by user's 'escape'


public slots:
    
};

#endif // QCAMERACAPTURE_H
