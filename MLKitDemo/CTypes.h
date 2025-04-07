//
//  CTypes.h
//  MLKitDemo
//
//  Created by Tùng on 31/10/2022.
//

//#ifndef CTypes_h
//#define CTypes_h
//
//
//#endif /* CTypes_h */

#ifndef CTYPES_HH
#define CTYPES_HH
#pragma once

#include <map>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <math.h>
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ostream>
#include <time.h>
#include <queue>
#include <float.h>
#include <limits.h>
#include <thread>
#include <zlib.h>
#include <complex>
#include <numeric>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif
#define NAPIEAR 2.718281828459045235360287471352
#define M_TWOPI 2.0*3.14159265358979323846264338327950288
#define EPSILON 1.0e-6
#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)
#define SIGNV(x) ((x>=0)?1.0:(-1.0))
#define SIGN(a,b) ((b)>0.0?fabs(a):-fabs(a))
#define SIGN(a) ((a)>=0.0?1:-1)

#if defined(__cplusplus) && __cplusplus < 201103L
    #ifdef _MSC_VER
        #define alignas(n)  __declspec(align(n))
    #else
        #define alignas(n)  __attribute__((aligned(n)))
    #endif
#endif

#define APPLE_CPU
#ifdef INTEL_CPU
    #include <xmmintrin.h>
    #include <immintrin.h>
    #define SIMD_ALIGN alignof(__m256)
#else
    #include <arm_neon.h>
    #define SIMD_ALIGN alignof(float32x4_t)
#endif
#define ALIGNED alignas(SIMD_ALIGN)


template <typename T> inline T* alignedMalloc(const std::size_t nBytes,const std::size_t alignment=SIMD_ALIGN) noexcept {
#if defined(_MSC_VER) || defined(__MINGW32__)
return(reinterpret_cast<T*>(_aligned_malloc(nBytes,alignment)));
#else
void* p;
return(reinterpret_cast<T*>(posix_memalign(&p,alignment,nBytes)==0?p:nullptr));
#endif
}

template <typename T> inline T* alignedAllocArray(const std::size_t size,const std::size_t alignment=SIMD_ALIGN) noexcept {
return(alignedMalloc<T>(size*sizeof(T),alignment));
}

template <typename T> inline void alignedFree(T* ptr) noexcept {
#if defined(_MSC_VER) || defined(__MINGW32__)
_aligned_free((void*)ptr);
#else
std::free((void*)ptr);
#endif
}


template <class R,class T,class S> struct OpAdd{
static inline R _apply(const T i,const S j){
return(i+j);
}
};

template <class R,class T,class S> struct OpSub{
static inline R _apply(const T i,const S j){
return(i-j);
}
};

template <class R,class T,class S> struct OpMul{
static inline R _apply(const T i,const S j){
return(i*j);
}
};

template <class R,class T,class S> struct OpDiv{
static inline R _apply(const T i,const S j){
return(i/j);
}
};

template <class R,class T> struct OpSquare{
static inline R _apply(const T i){
return(i*i);
}
};

template <class R,class T> struct OpSqrt{
static inline R _apply(const T i){
return(sqrt(i));
}
};

template <class R,class T> struct OpSin{
static inline R _apply(const T i){
return(sin(i));
}
};

template <class R,class T> struct OpCos{
static inline R _apply(const T i){
return(cos(i));
}
};

template <class R,class T> struct OpTan{
static inline R _apply(const T i){
return(tan(i));
}
};

template <class R,class T> struct OpTanh{
static inline R _apply(const T i){
return(tanh);
}
};

template <class R,class T> struct OpSigmoid{
static inline R _apply(const T i){
return(1.0/(1.0+exp(-i)));
}
};

template <class T> inline T square(T s){ return(s*s); }
template <class T> inline T square(T s1,T s2){ return(s1*s1+s2*s2); }
template <class T> inline T cubic(T s){ return(s*s*s); }


template <class T> class CArray{
public:

T* _instances;
int _size;

CArray():_size(0),_instances(nullptr){}
CArray(const int size):_instances(nullptr){ _resize(size); }

virtual ~CArray(){
if(_instances) delete [] _instances;
}

virtual void _resize(const int size){
if(_size==size) return;
if(_instances) delete [] _instances;
if(size>0) _instances=new T[size];
_size=size;
}


inline T* _getPointer(){ return(_instances); }


inline T* _get_instances(){ return(_instances); }
inline const T* _get_instances() const { return(_instances); }
inline int _get_size(){ return(_size); }
inline const int _get_size() const { return(_size); }
inline T& _get_data(int idx){ return(_instances[idx]); }
inline T& operator[](unsigned int i){ return(_instances[i]); }
inline const T& operator[](unsigned int i) const { return(_instances[i]); }


virtual void _delete(){


if(_instances) delete [] _instances;

_size=0;
_instances=nullptr;

}


virtual void _swap(CArray<T>& a){

T* tmp=a._instances;
a._instances=this->_instances;
this->_instances=tmp;

int size=a._size;
a._size=this->_size;
this->_size=size;

}

};


template <class T,class S> class CPair{
public:

T _first;
S _second;

CPair(){}
CPair(const CPair<T,S>& p){ _first=p._first; _second=p._second; }
CPair(const T t,const S s):_first(t),_second(s){}

inline void _set(T t,S s){ _first=t; _second=s; }
inline CPair<T,S>& operator=(double p){ return(*this); }
inline CPair<T,S>& operator=(CPair<T,S>& p){ _first=p._first; _second=p._second; return(*this); }

};


#endif
