//
//  CVector.h
//  MLKitDemo
//
//  Created by Tùng on 31/10/2022.
//
//
//#ifndef CVector_h
//#define CVector_h
//
//
//#endif /* CVector_h */


#include "CTypes.h"
#include <memory.h>
#include <cstdlib>


#ifndef CVECTOR_HH
#define CVECTOR_HH

template <class T> class CVector;

template <class T,class S> class CVectorContainer{
public:

inline const S _eval(const int i) const {
return((static_cast<const T*>(this))->_eval(i));
}

inline const unsigned int _get_size() const {
return((static_cast<const T*>(this))->_get_size());
}

inline const S* _get_val() const {
return((static_cast<const T*>(this))->_get_val());
}

inline const int _get_gpu() const {
return((static_cast<const T*>(this))->_get_gpu());
}

inline const CVector<S>* _get_vector() const {
return((static_cast<const T*>(this))->_get_vector());
}

};


template <class L,class Op,class R> class CVectorExpr:public CVectorContainer<CVectorExpr<L,Op,R>,typename R::value_type>{
const CVectorContainer<L,typename L::value_type> &_l;
const CVectorContainer<R,typename R::value_type> &_r;
public:

typedef typename R::value_type value_type;

CVectorExpr(const CVectorContainer<L,typename L::value_type>& l,const CVectorContainer<R,typename R::value_type>& r):_l(l),_r(r){}

inline const value_type _eval(const int i) const {
return(Op::_apply(_l._eval(i),_r._eval(i)));
}
  
inline const unsigned int _get_size() const { return(_r._get_size()); }
};

template <class Op,class R> class CVectorExprR:public CVectorContainer<CVectorExprR<Op,R>,typename R::value_type>{
const double &_l;
const CVectorContainer<R,typename R::value_type> &_r;
public:

typedef typename R::value_type value_type;

CVectorExprR(const double& l,const CVectorContainer<R,typename R::value_type>& r):_l(l),_r(r){}

inline const value_type _eval(const int i) const {
return(Op::_apply(_l,_r._eval(i)));
}
  
inline const unsigned int _get_size() const { return(_r._get_size()); }
};

template <class L,class Op> class CVectorExprL:public CVectorContainer<CVectorExprL<L,Op>,typename L::value_type>{
const CVectorContainer<L,typename L::value_type> &_l;
const double &_r;
public:

typedef typename L::value_type value_type;

CVectorExprL(const CVectorContainer<L,typename L::value_type>& l,const double& r):_l(l),_r(r){}

inline const value_type _eval(const int i) const {
return(Op::_apply(_l._eval(i),_r));
}
  
inline const unsigned int _get_size() const { return(_l._get_size()); }
};


// vector + vector
template <class L,class R> inline CVectorExpr<L,OpAdd<typename L::value_type,typename L::value_type,typename R::value_type>,R> operator+(const CVectorContainer<L,typename L::value_type>& lhs,const CVectorContainer<R,typename R::value_type>& rhs){
return(CVectorExpr<L,OpAdd<typename L::value_type,typename L::value_type,typename R::value_type>,R>(lhs,rhs));
}

// scalar + vector
template <class R> inline CVectorExprR<OpAdd<typename R::value_type,double,typename R::value_type>,R> operator+(const double& lhs,const CVectorContainer<R,typename R::value_type>& rhs){
return(CVectorExprR<OpAdd<typename R::value_type,double,typename R::value_type>,R>(lhs,rhs));
}

// vector + scalar
template <class L> inline CVectorExprL<L,OpAdd<typename L::value_type,typename L::value_type,double> > operator+(const CVectorContainer<L,typename L::value_type>& lhs,const double& rhs){
return(CVectorExprL<L,OpAdd<typename L::value_type,typename L::value_type,double> >(lhs,rhs));
}

// vector - vector
template <class L,class R> inline CVectorExpr<L,OpSub<typename L::value_type,typename L::value_type,typename R::value_type>,R> operator-(const CVectorContainer<L,typename L::value_type>& lhs,const CVectorContainer<R,typename R::value_type>& rhs){
return(CVectorExpr<L,OpSub<typename L::value_type,typename L::value_type,typename R::value_type>,R>(lhs,rhs));
}

// scalar - vector
template <class R> inline CVectorExprR<OpSub<typename R::value_type,double,typename R::value_type>,R> operator-(const double& lhs,const CVectorContainer<R,typename R::value_type>& rhs){
return(CVectorExprR<OpSub<typename R::value_type,double,typename R::value_type>,R>(lhs,rhs));
}

// vector - scalar
template <class L> inline CVectorExprL<L,OpSub<typename L::value_type,typename L::value_type,double> > operator-(const CVectorContainer<L,typename L::value_type>& lhs,const double& rhs){
return(CVectorExprL<L,OpSub<typename L::value_type,typename L::value_type,double> >(lhs,rhs));
}

// vector * vector
template <class L,class R> inline CVectorExpr<L,OpMul<typename L::value_type,typename L::value_type,typename R::value_type>,R> operator*(const CVectorContainer<L,typename L::value_type>& lhs,const CVectorContainer<R,typename R::value_type>& rhs){
return(CVectorExpr<L,OpMul<typename L::value_type,typename L::value_type,typename R::value_type>,R>(lhs,rhs));
}

// scalar * vector
template <class R> inline CVectorExprR<OpMul<typename R::value_type,double,typename R::value_type>,R> operator*(const double& lhs,const CVectorContainer<R,typename R::value_type>& rhs){
return(CVectorExprR<OpMul<typename R::value_type,double,typename R::value_type>,R>(lhs,rhs));
}

// vector * scalar
template <class L> inline CVectorExprL<L,OpMul<typename L::value_type,typename L::value_type,double> > operator*(const CVectorContainer<L,typename L::value_type>& lhs,const double& rhs){
return(CVectorExprL<L,OpMul<typename L::value_type,typename L::value_type,double> >(lhs,rhs));
}

// vector / vector
template <class L,class R> inline CVectorExpr<L,OpDiv<typename L::value_type,typename L::value_type,typename R::value_type>,R> operator/(const CVectorContainer<L,typename L::value_type>& lhs,const CVectorContainer<R,typename R::value_type>& rhs){
return(CVectorExpr<L,OpDiv<typename L::value_type,typename L::value_type,typename R::value_type>,R>(lhs,rhs));
}

// scalar / vector
template <class R> inline CVectorExprR<OpDiv<typename R::value_type,double,typename R::value_type>,R> operator/(const double& lhs,const CVectorContainer<R,typename R::value_type>& rhs){
return(CVectorExprR<OpDiv<typename R::value_type,double,typename R::value_type>,R>(lhs,rhs));
}

// vector / scalar
template <class L> inline CVectorExprL<L,OpDiv<typename L::value_type,typename L::value_type,double> > operator/(const CVectorContainer<L,typename L::value_type>& lhs,const double& rhs){
return(CVectorExprL<L,OpDiv<typename L::value_type,typename L::value_type,double> >(lhs,rhs));
}



template <class T> class CVector:public CVectorContainer<CVector<T>,T>{

public:

typedef T value_type;

T* _value=nullptr;
unsigned int _size=0;
int _ongpu=-1; // -1: no gpu, -2: simd
CVector<T> *_ref=nullptr;


CVector(){ _ref=this; }


explicit CVector(const unsigned int size,const bool simd=false){

_ref=this;
_size=size;
if(simd){
  _value=alignedAllocArray<T>(size);
  _ongpu=-2;
}else{
  _value=new T[size];
  _ongpu=-1;
}

}


template <class S> CVector(const CVectorContainer<S,typename S::value_type>& expr){

_ongpu=expr._get_gpu();
_ref=expr._get_vector();
_size=expr._get_size();
_value=expr._get_val();

}


template <class S> CVector(const CVectorContainer<S,typename S::value_type>& expr,const int offset){

_ongpu=expr._get_gpu();
_ref=expr._get_vector();
_size=expr._get_size()-offset;
_value=expr._get_val()+offset;

}


CVector(const unsigned int size,const T* ref){

_ref=nullptr;
_size=size;
_ongpu=-1;
_value=(T*)ref;

}


virtual ~CVector(){ _cleanup(); }


void _set(const unsigned int size,const T* ref){

_cleanup();
_ref=nullptr;
_size=size;
_ongpu=-1;
_value=(T*)ref;

}


virtual void _cleanup(){

if(_ref==this){
  if(_value){
    if(_ongpu<=-2) alignedFree(_value);
    else delete [] _value;
    _value=nullptr;
    _size=0;
  }
  _ongpu=-1;
}

_ref=nullptr;

}


inline T& operator[](const int i){ return(_value[i]); }
inline const T& operator[](const int i) const { return(_value[i]); }
inline const T _eval(const int i) const { return(_value[i]); }
inline const unsigned int _get_size() const { return(_size); }
inline const T* _get_val() const { return(_value); }
inline const int _get_gpu() const { return(_ongpu); }
inline const CVector<T>* _get_vector() const { return((_ref==this)?this:_ref); }
inline T* _getPointer(){ return(_value); }
inline const T* _getPointer() const { return(_value); }
const bool _useSIMD(){ return(_ongpu<=-2); }


void _resize(const unsigned int size,const bool simd=false){

if(size==0 || (_ref==this && size==_size && (simd && _ongpu<=-2))) return;

_cleanup();

T* tmp;

if(simd){
  tmp=alignedAllocArray<T>(size);
  _ongpu=-2;
}else{
  tmp=new T[size];
  _ongpu=-1;
}

_ref=this;
_size=size;
_value=tmp;

}


void _assign(const unsigned int size,const T* ptr){

_cleanup();

_ref=nullptr;
_ongpu=-1;
_value=ptr;
_size=size;

}


virtual void _swap(CVector<T>& v){

const unsigned int size=_size;
_size=v._size;
v._size=size;
T* tmp=_value;
_value=v._value;
v._value=tmp;
const int ongpu=v._ongpu;
v._ongpu=_ongpu;
_ongpu=ongpu;
CVector<T>* ref=_ref;
_ref=v._ref;
v._ref=ref;

}


void _swap(T* v){

T* tmp=_value;
_value=v;
v=tmp;

}


virtual const int _getGPU(const int idx=0) const {

if(_ongpu<-2) return(-_ongpu-3);

return(_ongpu);
}


CVector<T>& operator=(const T scalar){
for(int i=0;i<_size;++i){
   _value[i]=scalar;
}
return(*this);
}


CVector<T>& operator=(const T* v){
for(int i=0;i<_size;++i){
   _value[i]=v[i];
}
return(*this);
}


template <class S> inline CVector<T>& operator=(const CVectorContainer<S,typename S::value_type>& expr){

if(_ref==this && _size!=expr._get_size()){
  if(!_value){
    _size=expr._get_size();
    _value=new T[_size];
    _ongpu=-1;
  }else if(_size!=expr._get_size()){
    if(_ongpu<=-2){
      alignedFree(_value);
    }else{
      if(_ongpu>=-1) delete [] _value;
    }
    _ongpu=-1;
    _size=expr._get_size();
    _value=new T[_size];
  }
}else if(_size!=expr._get_size()){
  std::cout<<"vector assignment error"<<std::endl;
  //exit(-1);
}

for(int i=0;i<_size;++i){
   _value[i]=expr._eval(i);
}
return(*this);
}


template <class L,class Op,class R> inline CVector<T>& operator=(const CVectorExpr<L,Op,R>& expr){

if(_ref==this && _size!=expr._get_size()){
  if(!_value){
    _size=expr._get_size();
    _value=new T[_size];
    _ongpu=-1;
  }else if(_size!=expr._get_size()){
    if(_ongpu<=-2){
      alignedFree(_value);
    }else{
      if(_ongpu>=-1) delete [] _value;
    }
    _ongpu=-1;
    _size=expr._get_size();
    _value=new T[_size];
  }
}else if(_size!=expr._get_size()){
  std::cout<<"vector assignment error"<<std::endl;
  //exit(-1);
}

for(int i=0;i<_size;++i){
   _value[i]=expr._eval(i);
}
return(*this);
}


template <class Op,class R> inline CVector<T>& operator=(const CVectorExprR<Op,R>& expr){

if(_ref==this && _size!=expr._get_size()){
  if(!_value){
    _size=expr._get_size();
    _value=new T[_size];
    _ongpu=-1;
  }else if(_size!=expr._get_size()){
    if(_ongpu<=-2){
      alignedFree(_value);
    }else{
      if(_ongpu>=-1) delete [] _value;
    }
    _ongpu=-1;
    _size=expr._get_size();
    _value=new T[_size];
  }
}else if(_size!=expr._get_size()){
  std::cout<<"vector assignment error"<<std::endl;
  //exit(-1);
}

for(int i=0;i<_size;++i){
   _value[i]=expr._eval(i);
}
return(*this);
}


template <class L,class Op> inline CVector<T>& operator=(const CVectorExprL<L,Op>& expr){

if(_ref==this && _size!=expr._get_size()){
  if(!_value){
    _size=expr._get_size();
    _value=new T[_size];
    _ongpu=-1;
  }else if(_size!=expr._get_size()){
    if(_ongpu<=-2){
      alignedFree(_value);
    }else{
      if(_ongpu>=-1) delete [] _value;
    }
    _ongpu=-1;
    _size=expr._get_size();
    _value=new T[_size];
  }
}else if(_size!=expr._get_size()){
  std::cout<<"vector assignment error"<<std::endl;
  //exit(-1);
}

for(int i=0;i<_size;++i){
   _value[i]=expr._eval(i);
}
return(*this);
}


inline CVector<T>& operator=(const CVector<T>& expr){

if(_ref==this && _size!=expr._get_size()){
  if(!_value){
    _size=expr._get_size();
    _value=new T[_size];
    _ongpu=-1;
  }else if(_size!=expr._get_size()){
    if(_ongpu<=-2){
      alignedFree(_value);
    }else{
      if(_ongpu>=-1) delete [] _value;
    }
    _ongpu=-1;
    _size=expr._get_size();
    _value=new T[_size];
  }
}else if(_size!=expr._get_size()){
  std::cout<<"vector assignment error"<<std::endl;
  //exit(-1);
}

for(int i=0;i<_size;++i){
   _value[i]=expr._eval(i);
}
return(*this);
}



inline CVector<T>& operator<<(int s){
for(int i=0;i<_size;++i){
   _value[i]<<s;
}
return(*this);
}


inline CVector<T>& operator>>(int s){
for(int i=0;i<_size;++i){
   _value[i]>>s;
}
return(*this);
}


template <class L,class Op,class R> inline CVector<T>& operator+=(const CVectorExpr<L,Op,R>& expr){
for(int i=0;i<_size;++i){
   _value[i]+=expr._eval(i);
}
return(*this);
}

template <class Op,class R> inline CVector<T>& operator+=(const CVectorExprR<Op,R>& expr){
for(int i=0;i<_size;++i){
   _value[i]+=expr._eval(i);
}
return(*this);
}

template <class L,class Op> inline CVector<T>& operator+=(const CVectorExprL<L,Op>& expr){
for(int i=0;i<_size;++i){
   _value[i]+=expr._eval(i);
}
return(*this);
}

inline CVector<T>& operator+=(const CVector<T>& v){
for(int i=0;i<_size;++i){
   _value[i]+=v._eval(i);
}
return(*this);
}

inline CVector<T>& operator+=(const T* v){
for(int i=0;i<_size;++i){
   _value[i]+=v[i];
}
return(*this);
}

inline CVector<T>& operator+=(const T v){
for(int i=0;i<_size;++i){
   _value[i]+=v;
}
return(*this);
}


template <class L,class Op,class R> inline CVector<T>& operator-=(const CVectorExpr<L,Op,R>& expr){
for(int i=0;i<_size;++i){
   _value[i]-=expr._eval(i);
}
return(*this);
}

template <class Op,class R> inline CVector<T>& operator-=(const CVectorExprR<Op,R>& expr){
for(int i=0;i<_size;++i){
   _value[i]-=expr._eval(i);
}
return(*this);
}

template <class L,class Op> inline CVector<T>& operator-=(const CVectorExprL<L,Op>& expr){
for(int i=0;i<_size;++i){
   _value[i]-=expr._eval(i);
}
return(*this);
}

inline CVector<T>& operator-=(const CVector<T>& v){
for(int i=0;i<_size;++i){
   _value[i]-=v._eval(i);
}
return(*this);
}

inline CVector<T>& operator-=(const T* v){
for(int i=0;i<_size;++i){
   _value[i]-=v[i];
}
return(*this);
}

inline CVector<T>& operator-=(const T v){
for(int i=0;i<_size;++i){
   _value[i]-=v;
}
return(*this);
}


template <class L,class Op,class R> inline CVector<T>& operator*=(const CVectorExpr<L,Op,R>& expr){
for(int i=0;i<_size;++i){
   _value[i]*=expr._eval(i);
}
return(*this);
}

template <class Op,class R> inline CVector<T>& operator*=(const CVectorExprR<Op,R>& expr){
for(int i=0;i<_size;++i){
   _value[i]*=expr._eval(i);
}
return(*this);
}

template <class L,class Op> inline CVector<T>& operator*=(const CVectorExprL<L,Op>& expr){
for(int i=0;i<_size;++i){
   _value[i]*=expr._eval(i);
}
return(*this);
}

inline CVector<T>& operator*=(const CVector<T>& v){
for(int i=0;i<_size;++i){
   _value[i]*=v._eval(i);
}
return(*this);
}

inline CVector<T>& operator*=(const T* v){
for(int i=0;i<_size;++i){
   _value[i]*=v[i];
}
return(*this);
}

inline CVector<T>& operator*=(const T v){
for(int i=0;i<_size;++i){
   _value[i]*=v;
}
return(*this);
}


template <class L,class Op,class R> inline CVector<T>& operator/=(const CVectorExpr<L,Op,R>& expr){
for(int i=0;i<_size;++i){
   _value[i]/=expr._eval(i);
}
return(*this);
}

template <class Op,class R> inline CVector<T>& operator/=(const CVectorExprR<Op,R>& expr){
for(int i=0;i<_size;++i){
   _value[i]/=expr._eval(i);
}
return(*this);
}

template <class L,class Op> inline CVector<T>& operator/=(const CVectorExprL<L,Op>& expr){
for(int i=0;i<_size;++i){
   _value[i]/=expr._eval(i);
}
return(*this);
}

inline CVector<T>& operator/=(const CVector<T>& v){
for(int i=0;i<_size;++i){
   _value[i]/=v._eval(i);
}
return(*this);
}

inline CVector<T>& operator/=(const T* v){
for(int i=0;i<_size;++i){
   _value[i]/=v[i];
}
return(*this);
}

inline CVector<T>& operator/=(const T v){
for(int i=0;i<_size;++i){
   _value[i]/=v;
}
return(*this);
}


virtual void _zero(const bool gpu=false){

memset((void*)_value,0,_size*sizeof(T));

}


void _fill(const float x,const bool gpu=false){

for(int i=0;i<_size;++i) _value[i]=x;

}


void _range(){

for(int i=0;i<_size;++i) _value[i]=i;

}


void _shuffle(){

for(int i=0;i<_size;++i){
   const T tmp=_value[i];
   const int idx=rand()%_size;
   _value[i]=_value[idx];
   _value[idx]=tmp;
}

}

void _invert(){

for(int i=1;i<_size;++i){
   _value[i]=-_value[i];
}

}


const int _find(const T val){
for(int i=0;i<_size;++i){
   if(_value[i]==val) return(i);
}
return(-1);
}


inline T _max(){
T m=_value[0];
for(int i=1;i<_size;++i){
   if(_value[i]>m) m=_value[i];
}
return(m);
}


inline T _absmax(){
T m=fabs(_value[0]);
for(int i=1;i<_size;++i){
   T p=fabs(_value[i]);
   if(p>m) m=p;
}
return(m);
}


inline T _min(){
T m=_value[0];
for(int i=1;i<_size;++i){
   if(_value[i]<m) m=_value[i];
}
return(m);
}

inline int _argmax(){
T m=_value[0];
int r=0;
for(int i=1;i<_size;++i){
   if(_value[i]>m){
     m=_value[i];
     r=i;
   }
}
return(r);
}

inline int _argmin(){
T m=_value[0];
int r=0;
for(int i=1;i<_size;++i){
   if(_value[i]<m){
     m=_value[i];
     r=i;
   }
}
return(r);
}


inline void _max(T val){
for(int i=0;i<_size;++i){
   _value[i]=fmax(val,_value[i]);
}
}

inline void _min(T val){
for(int i=0;i<_size;++i){
   _value[i]=fmin(val,_value[i]);
}
}


inline T _sum(){
T s=0;
for(int i=0;i<_size;++i) s+=_value[i];
return(s);
}


inline T _mean(){
T s=0;
for(int i=0;i<_size;++i) s+=_value[i];
return(s/(T)_size);
}

inline T _var(const T mean){
T s=0;
for(int i=0;i<_size;++i) s+=square(_value[i]-mean);
return(s/(T)_size);
}

inline T _norm(){
T d=0;
for(int i=0;i<_size;++i) d+=_value[i]*_value[i];
return(sqrt(d));
}

inline T _normSquare(){
T d=0;
for(int i=0;i<_size;++i) d+=_value[i]*_value[i];
return(d);
}

inline void _abs(){
for(int i=0;i<_size;++i) _value[i]=fabs(_value[i]);
}

inline void _sqrt(){
for(int i=0;i<_size;++i) _value[i]=sqrt(_value[i]);
}

inline void _square(){
for(int i=0;i<_size;++i) _value[i]=square(_value[i]);
}

inline void _pow(const T p){
for(int i=0;i<_size;++i) _value[i]=pow(_value[i],p);
}

inline void _exp(){
for(int i=0;i<_size;++i) _value[i]=exp(_value[i]);
}

inline void _sin(){
for(int i=0;i<_size;++i) _value[i]=sin(_value[i]);
}

inline void _cos(){
for(int i=0;i<_size;++i) _value[i]=cos(_value[i]);
}

inline void _tan(){
for(int i=0;i<_size;++i) _value[i]=tan(_value[i]);
}

inline void _log(){
for(int i=0;i<_size;++i) _value[i]=log(_value[i]);
}

inline void _log10(){
for(int i=0;i<_size;++i) _value[i]=log10(_value[i]);
}

inline void _tanh(){
for(int i=0;i<_size;++i) _value[i]=tanh(_value[i]);
}

inline void _dtanh(){
for(int i=0;i<_size;++i){
   T s=tanh(_value[i]);
   _value[i]=1-s*s;
}
}


inline void _relu(){
for(int i=0;i<_size;++i){
   _value[i]=fmax(0,_value[i]);
}
}

inline void _sigmoid(){
for(int i=0;i<_size;++i) _value[i]=0.5*tanh(0.5*_value[i])+0.5;
}

inline void _clamp(const T min,const T max){
for(int i=0;i<_size;++i){
   if(_value[i]<min) _value[i]=min;
   else if(_value[i]>max) _value[i]=max;
}
}


inline void _random(const T min,const T max){
for(int i=0;i<_size;++i){
   _value[i]=min+(max-min)*(rand()/(double)RAND_MAX);
}
}


inline void _randint(const int p){
for(int i=0;i<_size;++i){
   _value[i]=rand()%p;
}
}


inline T _normalize(){
T d=0;
for(int i=0;i<_size;++i) d+=_value[i]*_value[i];
d=1.0/sqrt(d);
for(int i=0;i<_size;++i) _value[i]*=d;
return(d);
}


void _regularize(){

float maxf=-FLT_MAX;
float minf=FLT_MAX;
for(int i=0;i<_size;i++){
   if(maxf<_value[i]) maxf=_value[i];
   if(minf>_value[i]) minf=_value[i];
}
for(int i=0;i<_size;i++){
   _value[i]=2*((_value[i]-minf)/(maxf-minf)-0.5);
}

}


void _inv(){
for(int i=0;i<_size;++i) _value[i]=1.0/_value[i];
}

void _gram_schmidt(CVector<T>& v){
T d=_dot(v);
for(int i=0;i<_size;++i){
   v[i]-=d*_value[i];
}
}


inline void _load(std::ifstream& ifs,bool simd=false){

unsigned int n;
ifs.read((char*)&n,sizeof(unsigned int));

_resize(n,simd);
ifs.read((char*)_getPointer(),_size*sizeof(T));

}


inline void _save(std::ofstream& ofs){

ofs.write((char*)&_size,sizeof(unsigned int));
ofs.write((char*)_getPointer(),_size*sizeof(T));

}


};


template <class T> void normDeriv(const T n,CVector<T>& L,CVector<T>& x,CVector<T>& dx){

const int size=x._size;
T s=0;
for(int i=0;i<size;++i) s-=L[i]*x[i];
for(int i=0;i<size;++i) dx[i]=(s*x[i]+L[i])/n;

}



template <class T> void move(CVector<unsigned char>& src,CVector<T>& dst){

const size_t w=sizeof(T);

if(dst._size>0) dst._cleanup();

dst._size=src._size/w;
dst._value=(T*)src._getPointer();
dst._ongpu=src._ongpu;

src._size=0;
src._value=nullptr;
src._ongpu=-1;

}



template <class L,class R> inline double dot(CVectorContainer<L,typename L::value_type>& v1,CVectorContainer<R,typename R::value_type>& v2){
double d=0;
int s=v1._get_size();
for(int i=0;i<s;++i) d+=v1._eval(i)*v2._eval(i);
return(d);
}


template <class T> inline T dist(CVector<T>& v1,CVector<T>& v2){
T d=0;
const int s=v1._get_size();
for(int i=0;i<s;++i) d+=(v1._eval(i)-v2._eval(i))*(v1._eval(i)-v2._eval(i));
return(sqrt(d));
}

template <class T> inline T distSquare(CVector<T>& v1,CVector<T>& v2){
T d=0;
const int s=v1._get_size();
for(int i=0;i<s;++i) d+=(v1._eval(i)-v2._eval(i))*(v1._eval(i)-v2._eval(i));
return(d);
}


inline unsigned int hammingdist(CVector<char>& v1,CVector<char>& v2){
unsigned int d=0;
const int s=v1._get_size();
for(int i=0;i<s;++i){
   char tmp=(v1._eval(i))^(v2._eval(i));
   for(int j=0;j<8;++j){
      d+=tmp&0x01;
      tmp>>=1;
   }
}
return(d);
}

inline unsigned int hammingdist(CVector<unsigned char>& v1,CVector<unsigned char>& v2){
unsigned int d=0;
const int s=v1._get_size();
for(int i=0;i<s;++i){
   unsigned char tmp=(v1._eval(i))^(v2._eval(i));
   for(int j=0;j<8;++j){
      d+=tmp&0x01;
      tmp>>=1;
   }
}
return(d);
}


template <class T> void concat(CVector<T>& a,CVector<T>& b,CVector<T>& x){

if(x._size!=a._size+b._size) x._resize(a._size+b._size);
for(int i=0;i<a._size;++i) x[i]=a[i];
for(int i=0;i<b._size;++i) x[a._size+i]=b[i];

}



template <class T> void split(CVector<T>& x,CVector<T>& a,CVector<T>& b){

for(int i=0;i<a._size;++i) a[i]=x[i];
for(int i=0;i<b._size;++i) b[i]=x[a._size+i];

}




template <class T> void complexMul(CVector<T>& A,CVector<T>& B,CVector<T>& C){

const int n=A._size/2;
for(int i=0;i<n;++i){
   C[2*i]=A[2*i]*B[2*i]-A[2*i+1]*B[2*i+1];
   C[2*i+1]=A[2*i]*B[2*i+1]+A[2*i+1]*B[2*i];
}

}


template <class T> void linspace(const T min,const T max,const int n,CVector<T>& array){

if(n<2){
  std::cout<<"lispace error"<<std::endl;
  exit(-1);
}

if(array._size!=n) array._resize(n);

const T d=(max-min)/(T)(n-1);
for(int i=0;i<n;++i) array[i]=min+d*i;

}


template <class T> void cumsum(CVector<T>& x,CVector<T>& s){

s[0]=x[0];
for(int i=1;i<x._size;++i){
   s[i]=s[i-1]+x[i];
}

}


template <class T> int searchsorted(CVector<T>& vec,T x,bool right=false){


T n=vec._normSquare();
if(n==0 || n!=n) return(0);

vec*=(1.0/vec[vec._size-1]);

if(1){

  if(right){
    for(int i=vec._size-1;i>=0;--i){
       if(vec[i]<=x) return(i+1);
    }
    return(0);
  }else{
    for(int i=0;i<vec._size;++i){
       if(vec[i]>=x) return(i);
    }
    return(vec._size);
  }

}else{

  int lower=0;
  int upper=vec._size-1;
  int pivot=(lower+upper)/2;
  while(lower<upper){
       if(vec[pivot]==x) break;
       else if(vec[pivot]<x){
         lower=pivot+1;
         pivot=(lower+upper)/2;
       }else if(vec[pivot]>x){
         upper=pivot-1;
         pivot=(lower+upper)/2;
       }
  }

  if(right){
    for(int i=pivot;i<vec._size;++i){
       if(vec[i]>x) return(i);
    }
    return(vec._size);
  }else{
    for(int i=pivot;i>=0;--i){
       if(vec[i]<x) return(i+1);
    }
    return(0);
  }

}

return(0);
}



size_t loadNumpy(const std::string& filename,std::vector<int>& shape,CVector<unsigned char>& data);
size_t loadNumpy(const std::string& filename,char* data);
void loadNPZ(std::string filename,std::map<std::string,std::vector<int>>& shape,std::map<std::string,CVector<unsigned char>>& data);


template <typename T> std::vector<char>& operator+=(std::vector<char>& lhs,const T rhs){
for(size_t byte=0;byte<sizeof(T);byte++){
   char val=*((char*)&rhs+byte);
   lhs.push_back(val);
}
return(lhs);
}

std::vector<char>& operator+=(std::vector<char>& lhs,const std::string rhs);
std::vector<char>& operator+=(std::vector<char>& lhs,const char* rhs);
char BigEndianTest();
char map_type(const std::type_info& t);
void parse_npz_footer(FILE* fp,uint16_t& nrecs,size_t& global_header_size,size_t& global_header_offset);

template <typename T> std::vector<char> create_npy_header(const std::vector<size_t>& shape){

std::vector<char> dict;
dict+="{'descr': '";
dict+=BigEndianTest();
dict+=map_type(typeid(T));
dict+=std::to_string(sizeof(T));
dict+="', 'fortran_order': False, 'shape': (";
dict+=std::to_string(shape[0]);
for(size_t i=1;i<shape.size();i++){
   dict+=", ";
   dict+=std::to_string(shape[i]);
}
if(shape.size()==1) dict+=",";
dict+="), }";
//pad with spaces so that preamble+dict is modulo 16 bytes. preamble is 10 bytes. dict needs to end with \n
int remainder=16-(10+dict.size())%16;
dict.insert(dict.end(),remainder,' ');
dict.back()='\n';

std::vector<char> header;
header+=(char)0x93;
header+="NUMPY";
header+=(char)0x01; //major version of numpy format
header+=(char)0x00; //minor version of numpy format
header+=(uint16_t)dict.size();
header.insert(header.end(),dict.begin(),dict.end());

return(header);
}


template <typename T> void saveNPZ(std::string filename,std::string varname,const T* data,const std::vector<size_t>& shape,const bool add=false){
 
varname+=".npy";
FILE* fp=NULL;
uint16_t nrecs=0;
size_t global_header_offset=0;
std::vector<char> global_header;
if(add) fp=fopen(filename.c_str(),"r+b");

if(fp){
  size_t global_header_size;
  parse_npz_footer(fp,nrecs,global_header_size,global_header_offset);
  fseek(fp,global_header_offset,SEEK_SET);
  global_header.resize(global_header_size);
  size_t res = fread(&global_header[0],sizeof(char),global_header_size,fp);
  if(res!=global_header_size){
    throw std::runtime_error("npz_save: header read error while adding to existing zip");
  }
  fseek(fp,global_header_offset,SEEK_SET);
}else{
  fp=fopen(filename.c_str(),"wb");
}

std::vector<char> npy_header=create_npy_header<T>(shape);

size_t nels=std::accumulate(shape.begin(),shape.end(),1,std::multiplies<size_t>());
size_t nbytes=nels*sizeof(T) + npy_header.size();

//get the CRC of the data to be added
uint32_t crc=crc32(0L,(uint8_t*)&npy_header[0],npy_header.size());
crc=crc32(crc,(uint8_t*)data,nels*sizeof(T));

std::vector<char> local_header;
local_header+="PK"; //first part of sig
local_header+=(uint16_t)0x0403; //second part of sig
local_header+=(uint16_t)20; //min version to extract
local_header+=(uint16_t)0; //general purpose bit flag
local_header+=(uint16_t)0; //compression method
local_header+=(uint16_t)0; //file last mod time
local_header+=(uint16_t)0;     //file last mod date
local_header+=(uint32_t)crc; //crc
local_header+=(uint32_t)nbytes; //compressed size
local_header+=(uint32_t)nbytes; //uncompressed size
local_header+=(uint16_t)varname.size(); //fname length
local_header+=(uint16_t)0; //extra field length
local_header+=varname;

global_header+="PK"; //first part of sig
global_header+=(uint16_t)0x0201; //second part of sig
global_header+=(uint16_t)20; //version made by
global_header.insert(global_header.end(),local_header.begin()+4,local_header.begin()+30);
global_header+=(uint16_t)0; //file comment length
global_header+=(uint16_t)0; //disk number where file starts
global_header+=(uint16_t)0; //internal file attributes
global_header+=(uint32_t)0; //external file attributes
global_header+=(uint32_t)global_header_offset; //relative offset of local file header, since it begins where the global header used to begin
global_header+=varname;

std::vector<char> footer;
footer+="PK"; //first part of sig
footer+=(uint16_t)0x0605; //second part of sig
footer+=(uint16_t)0; //number of this disk
footer+=(uint16_t)0; //disk where footer starts
footer+=(uint16_t)(nrecs+1); //number of records on this disk
footer+=(uint16_t)(nrecs+1); //total number of records
footer+=(uint32_t)global_header.size(); //nbytes of global headers
footer+=(uint32_t)(global_header_offset + nbytes + local_header.size()); //offset of start of global headers, since global header now starts after newly written array
footer+=(uint16_t)0; //zip file comment length

fwrite(&local_header[0],sizeof(char),local_header.size(),fp);
fwrite(&npy_header[0],sizeof(char),npy_header.size(),fp);
fwrite(data,sizeof(T),nels,fp);
fwrite(&global_header[0],sizeof(char),global_header.size(),fp);
fwrite(&footer[0],sizeof(char),footer.size(),fp);
fclose(fp);

}



template <typename T> struct DescriptorDataType {};
template<> struct DescriptorDataType<double>{ static const char value='f'; };
template<> struct DescriptorDataType<float>{ static const char value='f'; };
template<> struct DescriptorDataType<int>{ static const char value='i'; };
template<> struct DescriptorDataType<short>{ static const char value='i'; };
template<> struct DescriptorDataType<unsigned short>{ static const char value='u'; };
template<> struct DescriptorDataType<char>{ static const char value='i'; };
template<> struct DescriptorDataType<unsigned char>{ static const char value='u'; };

void CreateMetaData(std::string& preamble,std::string& header,const std::string& descriptor,bool fortran_order,std::vector<int>& shape);

template <typename T> inline std::string CreateDescriptor(){

std::string descriptor;
descriptor.push_back('<');
descriptor.push_back(DescriptorDataType<T>::value);
std::stringstream stream;
stream<<sizeof(T);
descriptor.append(stream.str());

return(descriptor);
}


template <typename T> void saveNumpy(const std::string& filename,std::vector<int>& shape,const CVector<T>& data,bool fortran_order=false){

std::string preamble,header;
std::string descriptor=CreateDescriptor<T>();
CreateMetaData(preamble,header,descriptor,fortran_order,shape);
const size_t metadata_length=preamble.size()+header.size();
if(metadata_length%16!=0){
  throw std::runtime_error("formatting error: metadata length is not divisible by 16.");
}

std::ofstream stream(filename.c_str(),std::ios::out|std::ios::binary|std::ios::trunc);
if(!stream){
  throw std::runtime_error("io error: failed to open a file.");
}

stream<<preamble<<header;
int size=1;
for(int i=0;i<shape.size();++i){ size*=shape[i]; }
stream.write(reinterpret_cast<const char*>(data._getPointer()),sizeof(T)*size);

}




#endif

