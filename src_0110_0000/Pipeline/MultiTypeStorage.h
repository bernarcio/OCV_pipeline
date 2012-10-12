#ifndef MULTITYPESTORAGE_H
#define MULTITYPESTORAGE_H


#include <opencv2/opencv.hpp>
using namespace cv;

#include <typeinfo>
#include <vector>
#include <map>
#include <string>
using namespace std;


// idea is taken from boost::any

class PlaceHolder // abstract class  
{
public:
	virtual ~PlaceHolder(){}
	virtual const type_info & type() const = 0;
	virtual PlaceHolder * clone() const = 0;
};

template<typename ValueType>
class Holder : public PlaceHolder
{
public:
	Holder(const ValueType & v) : _item(v) {}
	PlaceHolder * clone() const {
		return new Holder(_item);
	}

	const type_info & type() const{
		return typeid(ValueType);
	}

 	ValueType _item;

private:
	ValueType & operator= (const ValueType & v);  // why is empty ???

};

class Any{

public:
	template<typename T>
	Any(const T & val) : content( new Holder<T>(val)) {}
	~Any(){ delete content; }

	template<typename T>
	void set(const T & v){
		if (content != NULL)
			delete content;
		content = new Holder<T>(v);
	}


	template<typename T>
	T * get(){
		if (strcmp(content->type().name(), typeid(T).name())==0){
			return &static_cast<Holder<T> * >(content)->_item;
		}else{
			return 0;
		}
	}

	const type_info & type() const {return content->type();}


private:
	PlaceHolder * content;
};



class MultiTypeStorage
{
	typedef Any StorageElementType;
	typedef Ptr<StorageElementType> StorageElementPtrType; // Here we use OpenCV  cv::Ptr smart-pointer
	typedef pair<string, StorageElementPtrType > StoragePairType;
	typedef map<string, StorageElementPtrType > StorageType;
	
	StorageType _storage;

public:
	MultiTypeStorage(){
		
	}
	~MultiTypeStorage(){
		// delete _storage: -> handled by Ptr<>
	}
	
	template<typename T>
	inline void setElement(string name, T & elmt){
		StorageType::iterator it = _storage.find(name);	
		if (it == _storage.end()){
			StorageElementPtrType element = new StorageElementType(elmt);
			_storage.insert(StoragePairType(name,element)); 
		}else{ 
			it->second = new StorageElementType(elmt); // --- Ptr behaviour : 	Any * A = new Any(a); Any * C = new Any(b);  Ptr<Any> B = A; B = C; // pointer to A is deleted : delete A -> error
		}
		
	}

	template<typename T>
	inline T * getElement(string name){
		StorageType::iterator it = _storage.find(name);
		return (it != _storage.end()) ? it->second->get<T>() : NULL; 
	}
	


	inline void copyStorageTo(MultiTypeStorage & MTStorage){
		StorageType::iterator iter = this->_storage.begin();
		for (;iter!=this->_storage.end();iter++){
			MTStorage._storage.insert(StoragePairType(iter->first, iter->second));
		}
	}


};
















#endif