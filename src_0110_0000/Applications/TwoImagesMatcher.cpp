
#include "OCV_pipeline.h"
#include "TwoImagesMatcher.h"



int TwoImagesMatcher_app()
{
	
	string path = "data/";
	string img1 = path + "image1.jpg";
	string img2 = path + "image2.jpg";
	TwoImagesMatcher twoImagesMatcher(img1,img2);

	CascadeDetectorFilter * faceDetector = new CascadeDetectorFilter(FACE, 2);
	//GenericDetectorFilter * fastL = new GenericDetectorFilter(GenericDetectorFilter::FAST, GenericDetectorFilter::NO, "learning_keypoints"); // fast and furious. good, a lot of keypoints
	HoughTransformationFilter * htL = new HoughTransformationFilter(HoughTransformationFilter::CIRCLES, true, 3, "learning_keypoints");
	
	// BRIEF - bad; ORB - not too bad; SIFT + BF_L1 -> drawMatches crushes
	GenericDescriptorExtractorFilter * descriptorL = new GenericDescriptorExtractorFilter(GenericDescriptorExtractorFilter::SURF, GenericDescriptorExtractorFilter::NO, "learning_keypoints", "learning_descriptors");
	
	//GenericDetectorFilter * fastM = new GenericDetectorFilter(GenericDetectorFilter::FAST, GenericDetectorFilter::NO, "matching_keypoints"); // fast and furious. good, a lot of keypoints
	HoughTransformationFilter * htM = new HoughTransformationFilter(HoughTransformationFilter::CIRCLES, true, 3, "matching_keypoints");
	GenericDescriptorExtractorFilter * descriptorM = new GenericDescriptorExtractorFilter(GenericDescriptorExtractorFilter::SURF, GenericDescriptorExtractorFilter::NO, "matching_keypoints", "matching_descriptors");
	GenericDescriptorMatcherFilter * matcher = new GenericDescriptorMatcherFilter(GenericDescriptorMatcherFilter::BRUTEFORCE_L1, GenericDescriptorMatcherFilter::MATCH, 1000.,  "matching_descriptors", "learning_descriptors");
	
	Pipeline learningPipeline, matchingPipeline;

	learningPipeline.addFilter(1, faceDetector); 
	//learningPipeline.addFilter(1, fastL);
	learningPipeline.addFilter(1, htL);
	learningPipeline.addFilter(1, descriptorL);

	// function to execute: GenericDescriptorMatcherFilter::addDescriptors(PipelineBuffer *)
	PipelineAbstractFunction * addDescriptorsCB = 
		new PipelineFunction1<GenericDescriptorMatcherFilter, PipelineBuffer>(PipelineAbstractFunction::CALLED_AFTER_FILTERS, matcher, &GenericDescriptorMatcherFilter::addDescriptors, learningPipeline.shareBuffer());
	// function to execute: GenericDescriptorMatcherFilter::trainMatcher()
	PipelineAbstractFunction * trainDescriptorsCB = 
		new PipelineFunction0<GenericDescriptorMatcherFilter>(PipelineAbstractFunction::CALLED_AFTER_FILTERS, matcher, &GenericDescriptorMatcherFilter::trainMatcher);
	
	string outputName = "learningPipeline_output";
	PipelineAbstractFunction * saveOutputImageCB = 
		new PipelineFunction2<PipelineBuffer, string, int>(PipelineAbstractFunction::CALLED_AFTER_FILTERS, learningPipeline.shareBuffer(), &PipelineBuffer::setInternalImage, outputName, 1);

	learningPipeline.addFunction(saveOutputImageCB);
	learningPipeline.addFunction(addDescriptorsCB);
	learningPipeline.addFunction(trainDescriptorsCB);


	// need to execute addDescriptors and trainDescriptor between two pipelines

	//matchingPipeline.addFilter(1, faceDetector); 
	//matchingPipeline.addFilter(1, fastM);
	matchingPipeline.addFilter(1, htM);
	matchingPipeline.addFilter(1, descriptorM);
	matchingPipeline.addFilter(1, matcher);
	

	twoImagesMatcher.setPipelines(&learningPipeline, &matchingPipeline);
	twoImagesMatcher.run();

	//system("PAUSE");
	return 0;

}



int TwoImagesMatcher_app2(){
	string path = "data/";
	string img1 = path + "image1.jpg";
	string img2 = path + "image2.jpg";
	string img3 = path + "image3.jpg";

	string templateName = "templ";

	TwoImagesMatcher twoImagesMatcher(img2,img1);

	// 1st image -> face + center(x,y) -> blur -> apply ellipse mask -> ... = descriptors
	// 2nd image -> blur -> window at center(x,y) -> apply ellipse mask -> compare descriptors -> result


	CascadeDetectorFilter * faceDetector = new CascadeDetectorFilter(FACE, 2, true, false, "face");
	BlurFilter * blur = new BlurFilter(BlurFilter::GAUSSIAN_BLUR, 7, 7, 5., 5.);

	MaskFilter * ellipseMask = new MaskFilter(MaskFilter::ELLIPSE, "face_0", true);
	MaskFilter * ellipseMask2 = new MaskFilter(MaskFilter::ELLIPSE, "face_0");
	MathOperationFilter * add = new MathOperationFilter(MathOperationFilter::AND);

	ImageAbstractFilter * threshold = new ThresholdFilter(ThresholdFilter::THRESH_BINARY, 120, 250);

	

	//ImageAbstractFilter * drawHist1 = new DrawHistogram(DrawHistogram::GRAY, "h1", 10);
	ImageAbstractFilter * matcher = new MatchTemplateFilter(MatchTemplateFilter::SQDIFF_NORMED, templateName);



	


	Pipeline learningPipeline, matchingPipeline;
	
	
	learningPipeline.addFilter(1, faceDetector);
	
		// !!! not the optimal way to apply a mask
		// there are other operations where a mask can be used
		// learningPipeline.addSplit(1);
		// learningPipeline.addFilter(2, ellipseMask);
		// PipelineInput in(1,2);
		// learningPipeline.addFilter(in, add);

	learningPipeline.addFilter(1, ellipseMask);
	learningPipeline.addFilter(1, threshold);	
	learningPipeline.addFilter(1, blur);
	

	
	PipelineAbstractFunction * saveTemplate = 
		new PipelineFunction2<PipelineBuffer, string, int>(PipelineAbstractFunction::CALLED_AFTER_FILTERS, learningPipeline.shareBuffer(), &PipelineBuffer::setInternalImage, templateName, 1);

	learningPipeline.addFunction(saveTemplate);

	//learningPipeline.addFilter(1, drawHist1);

	matchingPipeline.addFilter(1, threshold);	
	matchingPipeline.addFilter(1, blur);
	matchingPipeline.addFilter(1, matcher);

	twoImagesMatcher.setPipelines(&learningPipeline, &matchingPipeline);
	twoImagesMatcher.run();

	//system("PAUSE");
	return 0;

}
