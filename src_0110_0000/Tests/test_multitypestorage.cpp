#include "test_multitypestorage.h"

int test_multitypestorage()
{
	// init some variables:
	Mat a(10,10,CV_32F, Scalar(0.3));
	Mat b(20,20,CV_32F, Scalar(1.));
	Mat c(10,10,CV_32FC3, Scalar(120.));

	double v(9.);
	double w(19.);

	vector<KeyPoint> kp, kp1;
	kp.push_back(KeyPoint(1.0,-1.0,20));
	kp.push_back(KeyPoint(2.0,21.0,10));

	kp1.push_back(KeyPoint(11.0,-1.0,20));
	kp1.push_back(KeyPoint(2.0,1.0,10));


	vector<vector<DMatch> > m;
	vector<DMatch> _m, _n; 
	_m.push_back(DMatch(1,2,10.)); 
	_m.push_back(DMatch(3,4,4.)); 
	_n.push_back(DMatch(6,2,1.)); 
	_n.push_back(DMatch(5,4,41.)); 
	m.push_back(_m);
	m.push_back(_n);
		

	
	// 0-test:
	//cerr << "test_compare(1,2) = " << test_compare(1,2) << " = 0 (false) " << endl;
	//cerr << "test_compare(1.,1.) = " << test_compare(1.,1.) << " = 1 (true) " << endl;
	//cerr << "test_compare(a,a) = " << test_compare(a,a) << " = 1 " << endl;
	//cerr << "test_compare(a,b) = " << test_compare(a,b) << " = 0 "<< endl;
	//cerr << "test_compare(c,c) = " << test_compare(c,c) << " = 1 "<< endl;
	//cerr << "test_compare(a,c) = " << test_compare(a,c) << " = 0 "<< endl;
	//cerr << "test_compare(v,v) = " << test_compare(v,v) << " = 1 "<< endl;
	//cerr << "test_compare(v,w) = " << test_compare(v,w) << " = 0 "<< endl;
	//cerr << "test_compare(kp,kp) = " << test_compare<KeyPoint>(kp,kp) << " = 1 "<< endl;
	//cerr << "test_compare(kp,kp1) = " << test_compare<KeyPoint>(kp,kp1) << " = 0 "<< endl;
	//cerr << "test_compare(m,m) = " << test_compare<vector<DMatch> >(m,m) << " = 1 "<< endl;
	//cerr << "test_compare(_m,_n) = " << test_compare<DMatch>(_m,_n) << " = 0 "<< endl;


	// init MTStorage:
	MultiTypeStorage storage;	

	// add elements:
	storage.setElement<Mat>("a", a);
	storage.setElement<Mat>("b", b);
	storage.setElement<Mat>("c", c);
	storage.setElement<double>("v", v);
	storage.setElement<double>("w", w);
	storage.setElement<vector<KeyPoint> >("kp", kp);
	storage.setElement<vector<vector<DMatch> > >("m", m);


	// get elements:
	Mat A;
	if (storage.getElement<Mat>("a") != NULL) 
		A = *(storage.getElement<Mat>("a"));
	else{
		cerr << "a is not found !" << endl;
		return -1;
	}

	Mat B; 
	if (storage.getElement<Mat>("b")!=NULL)
		B = *(storage.getElement<Mat>("b"));
	else{
		cerr << "b is not found !" << endl;
		return -1;
	}

	Mat C = *(storage.getElement<Mat>("c"));

	double V = *(storage.getElement<double>("v"));
	double W = *(storage.getElement<double>("w"));
	vector<KeyPoint> KP = *(storage.getElement<vector<KeyPoint> >("kp"));
	vector<vector<DMatch> > M = *(storage.getElement<vector<vector<DMatch> > >("m"));

	// compare:
	int s1 = test_compare(A,a) + test_compare(B,b) + test_compare(C,c) + test_compare(V,v) + test_compare(W,w) + test_compare(KP,kp) + test_compare(M,m);
	if (s1 == 7)
		cerr << "test OK" << endl;
	else
		cerr << "test failed" << endl;


}

