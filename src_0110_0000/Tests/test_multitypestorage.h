#ifndef TEST_MULTYTYPESTORAGE_H
#define TEST_MULTYTYPESTORAGE_H

#include "multitypestorage.h"


int test_multitypestorage();


inline bool test_compare(int A, int B) {
	return A == B;
};

inline bool test_compare(float A, float B) {
	return A == B;
};

inline bool test_compare(double A, double B) {
	return ((A - B)*(A - B) < 1e-10) ? true : false;
};

inline bool test_compare(Mat & A, Mat & B){

	if (A.depth() == CV_64F && B.depth() == CV_64F && A.channels() == B.channels()){
		int c = A.channels();
		for (int row = 0; row < A.rows; row++){
			double * p1 = A.ptr<double>(row);
			double * p2 = B.ptr<double>(row);
			for (int col=0;col<A.cols*c;col++){
				if (p1[col] != p2[col])
					return false;
			}
		}

	}else if (A.depth() == CV_32F && B.depth() == CV_32F){
		int c = A.channels();
		for (int row = 0; row < A.rows; row++){
			float * p1 = A.ptr<float>(row);
			float * p2 = B.ptr<float>(row);
			for (int col=0;col<A.cols*c;col++){
				if (p1[col] != p2[col])
					return false;
			}
		}
	}else if (A.depth() == CV_8U && B.depth() == CV_8U){
		int c = A.channels();
		for (int row = 0; row < A.rows; row++){
			uchar * p1 = A.ptr<uchar>(row);
			uchar * p2 = B.ptr<uchar>(row);
			for (int col=0;col<A.cols*c;col++){
				if (p1[col] != p2[col])
					return false;
			}
		}
	}else{
		return false;	
	}

	return true;

}

inline bool test_compare(KeyPoint & A, KeyPoint & B){
	if (A.pt.x == B.pt.x)
		if (A.pt.y == B.pt.y)
			if (A.size == B.size)
				if (A.angle == B.angle)
					if (A.octave == B.octave)
						if (A.class_id == B.class_id)
							return true;

	return false;
}

inline bool test_compare(DMatch & A, DMatch & B){
	if (A.queryIdx == B.queryIdx)
		if(A.trainIdx == B.trainIdx)
			if (A.distance == B.distance)
				return true;

	return false;
}

template<typename T>
bool test_compare(vector<T> & A, vector<T> & B){
	vector<T>::iterator iter1 = A.begin();
	vector<T>::iterator iter2 = B.begin();
	for (;iter1 != A.end() || iter2 != B.end(); iter1++, iter2++){
		if (!test_compare(*iter1, *iter2))
			return false;
	}
	return true;
}




#endif