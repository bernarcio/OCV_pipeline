#ifndef OCV_PIPELINE_H
#define OCV_PIPELINE_H

// camera:
#include "CameraCapture_VideoInput.h"
#include "KeyboardHandle.h"

#ifdef Q_WS_W32
	#include "QCameraCapture.h
#endif

// Video AVi file as input:
#include "VideoReader.h"


// filters:
#include "BlurFilter.h"
#include "CalcBackProjectFilter.h"
#include "CalcOpticalFlowFilter.h"
#include "CamShiftFilter.h"
#include "CascadeDetectorFilter.h"
#include "ContourFilter.h"
#include "DrawContourFilter.h"
#include "DrawHistogram.h"
#include "DescriptorMatcherFilter.h"
#include "DynBckGrndSubtractionFilter.h"
#include "FIFOBufferFilter.h"
#include "GenericDetectorFilter.h"
#include "GenericDescriptorExtractorFilter.h"
#include "HistMotionDetectionFilter.h"
#include "HoughTransformationFilter.h"
#include "ImproveResolutionFilter.h"
#include "MatchTemplateFilter.h"
#include "MathOperationFilter.h"  // previously #include "MathBinaryOpFilter.h"
#include "MedianFilter.h"
#include "MorphoFilter.h"
#include "MotionDetectionFilter.h"
#include "MotionHistoryFilter.h"
#include "RescaleFilter.h"
#include "ResizeFilter.h"
#include "SegmentMotionFilter.h"
#include "SURFDetectorFilter.h"
#include "ThresholdFilter.h"

// pipeline:
#include "Pipeline.h"







#endif