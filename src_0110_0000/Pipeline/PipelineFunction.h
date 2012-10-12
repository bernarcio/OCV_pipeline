#ifndef PIPELINEFUNCTION_H
#define PIPELINEFUNCTION_H


// Abstract Function class to englobe all template PipelineFunctions
struct PipelineAbstractFunction {
public:
	enum Type {CALLED_AFTER_FILTERS, CALLED_BY_KEYHANDLER};
	PipelineAbstractFunction(Type ftype=CALLED_AFTER_FILTERS) : _ftype(ftype) {};
	virtual ~PipelineAbstractFunction(){};
	virtual void executeFunction() = 0;
	
	inline Type getFunctionType() {return _ftype;}

private:
	Type _ftype;
};




// T - template class. type1, type2 etc are template arguments. (VC++2010 no variadic templates support)
// support functions with at most 2 arguments:
template<class T>
class PipelineFunction0 : public PipelineAbstractFunction
{
	T * _classPtr;
	typedef void(T::*FunctionToCall_void)();
	FunctionToCall_void _function0;

public:
	

	PipelineFunction0(Type ftype, T* classPtr, FunctionToCall_void function) :
	  PipelineAbstractFunction(ftype), _classPtr(classPtr), _function0(function) {}
	  	
	inline virtual void executeFunction()
	{
			(_classPtr->*_function0)();
	}

	~PipelineFunction0(){};	
};

template<class T, class type1>
class PipelineFunction1 : public PipelineAbstractFunction
{
	T * _classPtr;
	typedef void(T::*FunctionToCall_onePtr)(type1 *);
	typedef void(T::*FunctionToCall_one)(type1);
	FunctionToCall_onePtr _function1Ptr;
	FunctionToCall_one _function1;
	type1 * _argument1Ptr;
	type1 _argument1;
	
	bool withPtr;


public:
	
	PipelineFunction1(Type ftype, T* classPtr, FunctionToCall_one function, type1 arg1) :
	  PipelineAbstractFunction(ftype),_classPtr(classPtr), _function1(function), _argument1(arg1) {
		withPtr = false;
	  }

		PipelineFunction1(Type ftype, T* classPtr, FunctionToCall_onePtr function, type1 * arg1) :
	  PipelineAbstractFunction(ftype),_classPtr(classPtr), _function1Ptr(function), _argument1Ptr(arg1) {
		withPtr = true;
	  }


	inline virtual void executeFunction()
	{	
		if(withPtr)
			(_classPtr->*_function1Ptr)(_argument1Ptr);
		else
			(_classPtr->*_function1)(_argument1);
	}

	~PipelineFunction1(){};

};

template<class T, class type1, class type2>
class PipelineFunction2 : public PipelineAbstractFunction
{
	T * _classPtr;
	typedef void(T::*FunctionToCall_two)(type1, type2);
	FunctionToCall_two _function2;
	type1 _argument1;
	type2 _argument2;


public:
	
	PipelineFunction2(Type ftype, T* classPtr, FunctionToCall_two function, type1 arg1, type2 arg2) :
	  PipelineAbstractFunction(ftype),_classPtr(classPtr), _function2(function), _argument1(arg1), _argument2(arg2) {}

	inline virtual void executeFunction()
	{
		(_classPtr->*_function2)(_argument1, _argument2);
	}

	~PipelineFunction2(){};


};



#endif

