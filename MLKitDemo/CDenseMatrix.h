//
//  CDenseMatrix.h
//  MLKitDemo
//
//  Created by Tùng on 31/10/2022.
//

//#ifndef CDenseMatrix_h
//#define CDenseMatrix_h
//
//
//#endif /* CDenseMatrix_h */


#include "CVector.h"

#ifndef CDENSEMATRIX_HH
#define CDENSEMATRIX_HH


template <class T,class S> class CDenseMatrixContainer{
public:

inline const S _eval(const int i,const int j) const {
return((static_cast<const T*>(this))->_eval(i,j));
}

inline const S _eval(const int i) const {
return((static_cast<const T*>(this))->_eval(i));
}

};


template <class L,class Op,class R> class CDenseMatrixExpr:public CDenseMatrixContainer<CDenseMatrixExpr<L,Op,R>,typename R::value_type>{
const CDenseMatrixContainer<L,typename L::value_type> &_l;
const CDenseMatrixContainer<R,typename R::value_type> &_r;
public:

typedef typename R::value_type value_type;

CDenseMatrixExpr(const CDenseMatrixContainer<L,typename L::value_type>& l,const CDenseMatrixContainer<R,typename R::value_type>& r):_l(l),_r(r){}

inline const value_type _eval(const int i,const int j) const {
return(Op::_apply(_l._eval(i,j),_r._eval(i,j)));
}

inline const value_type _eval(const int i) const {
return(Op::_apply(_l._eval(i),_r._eval(i)));
}
  
};


template <class Op,class R> class CDenseMatrixExprR:public CDenseMatrixContainer<CDenseMatrixExprR<Op,R>,typename R::value_type>{
const double &_l;
const CDenseMatrixContainer<R,typename R::value_type> &_r;
public:

typedef typename R::value_type value_type;

CDenseMatrixExprR(const double& l,const CDenseMatrixContainer<R,typename R::value_type>& r):_l(l),_r(r){}

inline const value_type _eval(const int i,const int j) const {
return(Op::_apply(_l,_r._eval(i,j)));
}

inline const value_type _eval(const int i) const {
return(Op::_apply(_l,_r._eval(i)));
}

};

template <class L,class Op> class CDenseMatrixExprL:public CDenseMatrixContainer<CDenseMatrixExprL<L,Op>,typename L::value_type>{
const CDenseMatrixContainer<L,typename L::value_type> &_l;
const double &_r;
public:

typedef typename L::value_type value_type;

CDenseMatrixExprL(const CDenseMatrixContainer<L,typename L::value_type>& l,const double& r):_l(l),_r(r){}

inline const value_type _eval(const int i,const int j) const {
return(Op::_apply(_l._eval(i,j),_r));
}

inline const value_type _eval(const int i) const {
return(Op::_apply(_l._eval(i),_r));
}
  
inline const int _get_size() const { return(_l._get_size()); }
};



// Matrix + Matrix
template <class L,class R> inline CDenseMatrixExpr<L,OpAdd<typename L::value_type,typename L::value_type,typename R::value_type>,R> operator+(const CDenseMatrixContainer<L,typename L::value_type>& lhs,const CDenseMatrixContainer<R,typename R::value_type>& rhs){
return(CDenseMatrixExpr<L,OpAdd<typename L::value_type,typename L::value_type,typename R::value_type>,R>(lhs,rhs));
}

// scalar + Matrix
template <class R> inline CDenseMatrixExprR<OpAdd<typename R::value_type,double,typename R::value_type>,R> operator+(const double& lhs,const CDenseMatrixContainer<R,typename R::value_type>& rhs){
return(CDenseMatrixExprR<OpAdd<typename R::value_type,double,typename R::value_type>,R>(lhs,rhs));
}

// Matrix + scalar
template <class L> inline CDenseMatrixExprL<L,OpAdd<typename L::value_type,typename L::value_type,double>> operator+(const CDenseMatrixContainer<L,typename L::value_type>& lhs,const double& rhs){
return(CDenseMatrixExprL<L,OpAdd<typename L::value_type,typename L::value_type,double> >(lhs,rhs));
}

// Matrix - Matrix
template <class L,class R> inline CDenseMatrixExpr<L,OpSub<typename L::value_type,typename L::value_type,typename R::value_type>,R> operator-(const CDenseMatrixContainer<L,typename L::value_type>& lhs,const CDenseMatrixContainer<R,typename R::value_type>& rhs){
return(CDenseMatrixExpr<L,OpSub<typename L::value_type,typename L::value_type,typename R::value_type>,R>(lhs,rhs));
}

// scalar - Matrix
template <class R> inline CDenseMatrixExprR<OpSub<typename R::value_type,double,typename R::value_type>,R> operator-(const double& lhs,const CDenseMatrixContainer<R,typename R::value_type>& rhs){
return(CDenseMatrixExprR<OpSub<typename R::value_type,double,typename R::value_type>,R>(lhs,rhs));
}

// Matrix - scalar
template <class L> inline CDenseMatrixExprL<L,OpSub<typename L::value_type,typename L::value_type,double>> operator-(const CDenseMatrixContainer<L,typename L::value_type>& lhs,const double& rhs){
return(CDenseMatrixExprL<L,OpSub<typename L::value_type,typename L::value_type,double> >(lhs,rhs));
}

// scalar * Matrix
template <class R> inline CDenseMatrixExprR<OpMul<typename R::value_type,double,typename R::value_type>,R> operator*(const double& lhs,const CDenseMatrixContainer<R,typename R::value_type>& rhs){
return(CDenseMatrixExprR<OpMul<typename R::value_type,double,typename R::value_type>,R>(lhs,rhs));
}

// Matrix * scalar
template <class L> inline CDenseMatrixExprL<L,OpMul<typename L::value_type,typename L::value_type,double>> operator*(const CDenseMatrixContainer<L,typename L::value_type>& lhs,const double& rhs){
return(CDenseMatrixExprL<L,OpMul<typename L::value_type,typename L::value_type,double> >(lhs,rhs));
}

// Matrix / Matrix
template <class L,class R> inline CDenseMatrixExpr<L,OpDiv<typename L::value_type,typename L::value_type,typename R::value_type>,R> operator/(const CDenseMatrixContainer<L,typename L::value_type>& lhs,const CDenseMatrixContainer<R,typename R::value_type>& rhs){
return(CDenseMatrixExpr<L,OpDiv<typename L::value_type,typename L::value_type,typename R::value_type>,R>(lhs,rhs));
}

// scalar / Matrix
template <class R> inline CDenseMatrixExprR<OpDiv<typename R::value_type,double,typename R::value_type>,R> operator/(const double& lhs,const CDenseMatrixContainer<R,typename R::value_type>& rhs){
return(CDenseMatrixExprR<OpDiv<typename R::value_type,double,typename R::value_type>,R>(lhs,rhs));
}

// Matrix / scalar
template <class L> inline CDenseMatrixExprL<L,OpAdd<typename L::value_type,typename L::value_type,double>> operator/(const CDenseMatrixContainer<L,typename L::value_type>& lhs,const double& rhs){
return(CDenseMatrixExprL<L,OpDiv<typename L::value_type,typename L::value_type,double> >(lhs,rhs));
}


template <class T> class CDenseMatrix:public CDenseMatrixContainer<CDenseMatrix<T>,T>{
public:

typedef T value_type;

unsigned int _row=0,_col;
T** _value=nullptr;
CVector<T> _pool;

CDenseMatrix(){ _value=NULL; }


CDenseMatrix(const unsigned int row,const unsigned int col,bool simd=false){

_row=row; _col=col;
_pool._resize(row*col,simd);
_value=new T*[row];
for(int i=0;i<row;++i){
   _value[i]=&_pool[col*i];
}

}


CDenseMatrix(const CDenseMatrix<T>& mat):_pool(mat._pool){

_row=mat._row; _col=mat._col;
_value=new T*[this->_row];
for(int i=0;i<this->_row;++i){
   _value[i]=&_pool[this->_col*i];
}

}


CDenseMatrix(const unsigned int row,const unsigned int col,const T* ptr):_pool(row*col,ptr){

_row=row; _col=col;
_value=new T*[row];
for(int i=0;i<row;++i){
   _value[i]=&_pool[col*i];
}

}


~CDenseMatrix(){

if(_value){
  delete [] _value;
}

}


virtual void _resize(const unsigned int row,const unsigned int col,bool simd=false){

if(this->_row==row && this->_col==col && simd==_pool._useSIMD()) return;

if(_value){
  delete [] _value;
}

_pool._resize(row*col,simd);
this->_row=row;
this->_col=col;
_value=new T*[row];
for(int i=0;i<row;++i){
   _value[i]=&_pool[col*i];
}

}


void _assign(T* ptr){

_pool._assign(_row*_col,ptr);

_value=new T*[_row];
for(int i=0;i<_row;++i){
   _value[i]=&_pool[_col*i];
}

}


void _swap(CDenseMatrix<T>& m){

_pool._swap(m._pool);
T* tmp;
for(int i=0;i<this->_row;++i){
   tmp=_value[i];
   _value[i]=m._value[i];
   m._value[i]=tmp;
}

}


void _delete(){

if(_value){
  delete [] _value;
  _value=nullptr;
  _pool._cleanup();
}

this->_row=0;
this->_col=0;

}


inline CVector<T>& _getVector(){ return(_pool); }
inline T* _getPointer(){ return(_pool._getPointer()); }
inline const T _eval(const int i,const int j) const { return(_value[i][j]); }
inline const T _eval(const int i) const { return(_pool[i]); }
inline const int _get_size() const { return(this->_row*this->_col); }

inline virtual T _get_value(int row,int col){
return(this->_value[row][col]);
}

inline T _maxDiag(){
T m=this->_value[0][0];
for(int i=1;i<this->_row;++i){
   if(this->_value[i][i]>m) m=this->_value[i][i];
}
return(m);
}

inline T _minDiag(){
T m=this->_value[0][0];
for(int i=1;i<this->_row;++i){
   if(this->_value[i][i]<m) m=this->_value[i][i];
}

return(m);
}


inline T _max(){
T m=this->_value[0][0];
for(int i=0;i<this->_row;++i){
   for(int j=0;j<this->_col;++j){
      if(this->_value[i][j]>m) m=this->_value[i][j];
   }
}
return(m);
}

inline T _min(){
T m=this->_value[0][0];
for(int i=0;i<this->_row;++i){
   for(int j=0;j<this->_col;++j){
      if(this->_value[i][j]<m) m=this->_value[i][j];
   }
}
return(m);
}


inline void _random(const T min,const T max){
for(int i=0;i<this->_row;++i){
   for(int j=0;j<this->_col;++j){
      this->_value[i][j]=random(min,max);
   }
}
}


inline T _mean(){

T m=0;
for(int i=0;i<this->_row;++i){
   for(int j=0;j<this->_col;++j){
      m+=this->_value[i][j];
   }
}
m/=(T)(this->_row*this->_col);

return(m);
}

inline void _addDiag(T val){
for(int i=0;i<this->_row;++i){
   this->_value[i][i]+=val;
}
}

inline void _subDiag(T val){
for(int i=0;i<this->_row;++i){
   this->_value[i][i]-=val;
}
}

inline void _diagonalize(){
for(int i=0;i<this->_row;++i){
   for(int j=0;j<i;++j) this->_value[i][j]=0;
   for(int j=i+1;j<this->_col;++j) this->_value[i][j]=0;
}
}



inline CDenseMatrix<T>& operator=(const double& scalar){
for(int i=0;i<this->_row;++i){
   for(int j=0;j<this->_col;++j){
      _value[i][j]=scalar;
   }
}
return(*this);
}

template <class S> inline CDenseMatrix<T>& operator=(const CDenseMatrixContainer<S,typename S::value_type>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]=expr._eval(i);
return(*this);
}

template <class L,class Op,class R> inline CDenseMatrix<T>& operator=(const CDenseMatrixExpr<L,Op,R>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]=expr._eval(i);
return(*this);
}

template <class Op,class R> inline CDenseMatrix<T>& operator=(const CDenseMatrixExprR<Op,R>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]=expr._eval(i);
return(*this);
}

template <class L,class Op> inline CDenseMatrix<T>& operator=(const CDenseMatrixExprL<L,Op>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]=expr._eval(i);
return(*this);
}

inline CDenseMatrix<T>& operator=(const CDenseMatrix<T>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]=expr._eval(i);
return(*this);
}


inline CDenseMatrix<T>& operator+=(const double& scalar){
_pool+=scalar;
return(*this);
}

template <class S> inline CDenseMatrix<T>& operator+=(const CDenseMatrixContainer<S,typename S::value_type>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]+=expr._eval(i);
return(*this);
}

template <class L,class Op,class R> inline CDenseMatrix<T>& operator+=(const CDenseMatrixExpr<L,Op,R>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]+=expr._eval(i);
return(*this);
}

template <class Op,class R> inline CDenseMatrix<T>& operator+=(const CDenseMatrixExprR<Op,R>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]+=expr._eval(i);
return(*this);
}

template <class L,class Op> inline CDenseMatrix<T>& operator+=(const CDenseMatrixExprL<L,Op>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]+=expr._eval(i);
return(*this);
}

template <class L,class Op,class R> inline CDenseMatrix<T>& operator+=(const CVectorExpr<L,Op,R>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]+=expr._eval(i);
   v+=_col;
}
return(*this);
}

template <class L,class Op> inline CDenseMatrix<T>& operator+=(const CVectorExprL<L,Op>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]+=expr._eval(i);
   v+=_col;
}
return(*this);
}

template <class Op,class R> inline CDenseMatrix<T>& operator+=(const CVectorExprR<Op,R>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]+=expr._eval(i);
   v+=_col;
}
return(*this);
}

inline CDenseMatrix<T>& operator+=(const CDenseMatrix<T>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]+=expr._eval(i);
return(*this);
}


inline CDenseMatrix<T>& operator-=(const double& scalar){
_pool-=scalar;
return(*this);
}

template <class S> inline CDenseMatrix<T>& operator-=(const CDenseMatrixContainer<S,typename S::value_type>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]-=expr._eval(i);
return(*this);
}

template <class L,class Op,class R> inline CDenseMatrix<T>& operator-=(const CDenseMatrixExpr<L,Op,R>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]-=expr._eval(i);
return(*this);
}

template <class Op,class R> inline CDenseMatrix<T>& operator-=(const CDenseMatrixExprR<Op,R>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]-=expr._eval(i);
return(*this);
}

template <class L,class Op> inline CDenseMatrix<T>& operator-=(const CDenseMatrixExprL<L,Op>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]-=expr._eval(i);
return(*this);
}

template <class L,class Op,class R> inline CDenseMatrix<T>& operator-=(const CVectorExpr<L,Op,R>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]-=expr._eval(i);
   v+=_col;
}
return(*this);
}

template <class L,class Op> inline CDenseMatrix<T>& operator-=(const CVectorExprL<L,Op>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]-=expr._eval(i);
   v+=_col;
}
return(*this);
}

template <class Op,class R> inline CDenseMatrix<T>& operator-=(const CVectorExprR<Op,R>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]-=expr._eval(i);
   v+=_col;
}
return(*this);
}

inline CDenseMatrix<T>& operator-=(const CDenseMatrix<T>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]-=expr._eval(i);
return(*this);
}


inline CDenseMatrix<T>& operator*=(const double scalar){
_pool*=scalar;
return(*this);
}

template <class S> inline CDenseMatrix<T>& operator*=(const CDenseMatrixContainer<S,typename S::value_type>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]*=expr._eval(i);
return(*this);
}

template <class L,class Op,class R> inline CDenseMatrix<T>& operator*=(const CDenseMatrixExpr<L,Op,R>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]*=expr._eval(i);
return(*this);
}

template <class Op,class R> inline CDenseMatrix<T>& operator*=(const CDenseMatrixExprR<Op,R>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]*=expr._eval(i);
return(*this);
}

template <class L,class Op> inline CDenseMatrix<T>& operator*=(const CDenseMatrixExprL<L,Op>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]*=expr._eval(i);
return(*this);
}

template <class L,class Op,class R> inline CDenseMatrix<T>& operator*=(const CVectorExpr<L,Op,R>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]*=expr._eval(i);
   v+=_col;
}
return(*this);
}

template <class L,class Op> inline CDenseMatrix<T>& operator*=(const CVectorExprL<L,Op>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]*=expr._eval(i);
   v+=_col;
}
return(*this);
}

template <class Op,class R> inline CDenseMatrix<T>& operator*=(const CVectorExprR<Op,R>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]*=expr._eval(i);
   v+=_col;
}
return(*this);
}

inline CDenseMatrix<T>& operator*=(const CDenseMatrix<T>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]*=expr._eval(i);
return(*this);
}


inline CDenseMatrix<T>& operator/=(const double& scalar){
_pool/=scalar;
return(*this);
}

template <class S> inline CDenseMatrix<T>& operator/=(const CDenseMatrixContainer<S,typename S::value_type>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]/=expr._eval(i);
}

template <class L,class Op,class R> inline CDenseMatrix<T>& operator/=(const CDenseMatrixExpr<L,Op,R>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]/=expr._eval(i);
return(*this);
}

template <class Op,class R> inline CDenseMatrix<T>& operator/=(const CDenseMatrixExprR<Op,R>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]/=expr._eval(i);
return(*this);
}

template <class L,class Op> inline CDenseMatrix<T>& operator/=(const CDenseMatrixExprL<L,Op>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]/=expr._eval(i);
return(*this);
}

template <class L,class Op,class R> inline CDenseMatrix<T>& operator/=(const CVectorExpr<L,Op,R>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]/=expr._eval(i);
   v+=_col;
}
return(*this);
}

template <class L,class Op> inline CDenseMatrix<T>& operator/=(const CVectorExprL<L,Op>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]/=expr._eval(i);
   v+=_col;
}
return(*this);
}

template <class Op,class R> inline CDenseMatrix<T>& operator/=(const CVectorExprR<Op,R>& expr){
T* v=_pool._getPointer();
for(int i=0;i<_row;++i){
   v[i]/=expr._eval(i);
   v+=_col;
}
return(*this);
}

inline CDenseMatrix<T>& operator/=(const CDenseMatrix<T>& expr){
for(int i=0;i<_pool._size;++i) _pool[i]/=expr._eval(i);
return(*this);
}



void _zero(const bool gpu=false){

_pool._zero(gpu);

}


T _sum(){

T s=0;
for(int j=0;j<this->_col;++j){
   for(int i=0;i<this->_row;++i) s+=_value[i][j];
}

return(s);
}


void _abs(){

for(int j=0;j<this->_col;++j){
   for(int i=0;i<this->_row;++i) _value[i][j]=fabs(_value[i][j]);
}

}

void _normalizeCol(){

for(int j=0;j<this->_col;++j){
   T n=0;
   for(int i=0;i<this->_row;++i) n+=_value[i][j]*_value[i][j];
   n=1.0/sqrt(n);
   for(int i=0;i<this->_row;++i) _value[i][j]*=n;
}

}


T _norm(){
return(_pool._norm());
}

void _pow(const T p){
for(int i=0;i<this->_row;++i){
   for(int j=0;j<this->_col;++j){
      _value[i][j]=pow(_value[i][j],p);
   }
}
}

void _exp(){
for(int i=0;i<this->_row;++i){
   for(int j=0;j<this->_col;++j){
      _value[i][j]=exp(_value[i][j]);
   }
}
}

void _square(){
for(int i=0;i<this->_row;++i){
   for(int j=0;j<this->_col;++j){
      _value[i][j]=square(_value[i][j]);
   }
}
}

void _log(){
for(int i=0;i<this->_row;++i){
   for(int j=0;j<this->_col;++j){
      _value[i][j]=log(_value[i][j]);
   }
}
}

void _log10(){
for(int i=0;i<this->_row;++i){
   for(int j=0;j<this->_col;++j){
      _value[i][j]=log10(_value[i][j]);
   }
}
}

void _sqrt(){
for(int i=0;i<this->_row;++i){
   for(int j=0;j<this->_col;++j){
      _value[i][j]=sqrt(_value[i][j]);
   }
}
}



void _identity(){
_zero();
for(int i=0;i<this->_row;i++){
   _value[i][i]=1;
}
}

bool _isSymmetric(){
for(int i=0;i<this->_row;++i){
   for(int j=0;j<i;++j){
      if(fabs(_value[i][j]-_value[j][i])>1.0e-3) return(false);
      
   }
}
return(true);
}

bool _isPositiveDiagonal(){
for(int i=0;i<this->_row;++i){
   if(_value[i][i]<0) return(false);
}
return(true);
}


inline T &operator()(int i,int j){
return(_value[i][j]);
}

inline const T &operator()(int i,int j) const {
return(_value[i][j]);
}

inline T* operator[](int i){
return(_value[i]);
}

inline const T* operator[](int i) const {
return(_value[i]);
}

inline void _set(int i,int j,T val){
_value[i][j]=val;
}

inline T *_get_vec(int row){
return(_value[row]);
}

inline void _copy_vec(int row,T* vec){
memcpy(_value[row],vec,this->_col*sizeof(T));
}



inline void _apply(const T *x,T *y) const {

const int row=this->_row;
const int col=this->_col;

for(int j=0;j<row;++j){
   T d;
   d=0;
   for(int i=0;i<col;++i){
      d+=_value[j][i]*x[i];
   }
   y[j]=d;
}

}


inline void _apply(const CVector<T>& x,CVector<T>& y) const {
const int row=this->_row;
const int col=this->_col;

for(int j=0;j<row;++j){
   T d;
   d=0;
   for(int i=0;i<col;++i){
      d+=_value[j][i]*x[i];
   }
   y[j]=d;
}

}

inline void _apply_add(const T *x,T *y) const {
const int row=this->_row;
const int col=this->_col;

for(int j=0;j<row;++j){
   T d;
   d=0;
   for(int i=0;i<col;++i){
      d+=_value[j][i]*x[i];
   }
   y[j]+=d;
}

}

inline void _apply_add(const CVector<T>& x,CVector<T>& y) const {
const int row=this->_row;
const int col=this->_col;

for(int j=0;j<row;++j){
   T d;
   d=0;
   for(int i=0;i<col;++i){
      d+=_value[j][i]*x[i];
   }
   y[j]+=d;
}

}


inline void _apply_transpose(const T *x,T *y) const {
const int row=this->_row;
const int col=this->_col;

for(int j=0;j<col;++j){
   T d;
   d=0;
   for(int i=0;i<row;++i){
      d+=_value[i][j]*x[i];
   }
   y[j]=d;
}

}

inline void _apply_transpose(const CVector<T>& x,CVector<T>& y) const {
const int row=this->_row;
const int col=this->_col;

for(int j=0;j<col;++j){
   T d;
   d=0;
   for(int i=0;i<row;++i){
      d+=_value[i][j]*x[i];
   }
   y[j]=d;
}

}


inline T _dotRow(int col,CVector<T>& vec){
T sum=0;
for(int i=0;i<this->_row;++i) sum+=_value[i][col]*vec[i];
return(sum);
}


void _printMatlab(){
std::cout<<"[";
for(int j=0;j<this->_row;++j){
   for(int i=0;i<this->_col;++i){
      std::cout<<_value[j][i]<<" ";
   }
   std::cout<<";";
}
std::cout<<"];";
}


void _symmetrize(bool upper=true){

if(upper){
  for(int j=0;j<this->_row;++j){
     for(int i=j+1;i<this->_col;++i){
        _value[i][j]=_value[j][i];
     }
  }
}else{
  for(int j=1;j<this->_row;++j){
     for(int i=0;i<j;++i){
        _value[i][j]=_value[j][i];
     }
  }
}

}

void _transpose(CDenseMatrix<T>& m){
for(int j=0;j<this->_row;++j){
   for(int i=0;i<this->_col;++i){
      m[i][j]=_value[j][i];
   }
}
}


T _trace(){
T d=0;
for(int j=0;j<this->_row;++j) d+=_value[j][j];
return(d);
}


inline void _load(std::ifstream& ifs,bool simd=false){

int r,c;
ifs.read((char*)&r,sizeof(int));
ifs.read((char*)&c,sizeof(int));
_resize(r,c,simd);
int n=this->_row*this->_col;
ifs.read((char*)&_pool[0],n*sizeof(T));

}


inline void _save(std::ofstream& ofs){

ofs.write((char*)&this->_row,sizeof(int));
ofs.write((char*)&this->_col,sizeof(int));
int n=this->_row*this->_col;
ofs.write((char*)&_pool[0],n*sizeof(T));

}


};


template <class T,class L,class R> inline void mul(CVectorContainer<L,typename L::value_type>& v1,CVectorContainer<R,typename R::value_type>& v2,CDenseMatrix<T>& res){
int row=v1._get_size();
int col=v2._get_size();
for(int j=0;j<row;++j){
   for(int i=0;i<col;++i){
      res[j][i]=v1._eval(j)*v2._eval(i);
   }
}
}


template <class T> inline void mul(CVector<T>& v,CDenseMatrix<T>& mat,CVector<T>& res){
const int s=v._size;
const int col=mat._col;

for(int i=0;i<col;++i){
   T d=0;
   for(int j=0;j<s;++j) d+=v[j]*mat[j][i];
   res[i]=d;
}

}



template <class T> inline void mul(CDenseMatrix<T>& mat1,CDenseMatrix<T>& mat2,CDenseMatrix<T>& res){
int row=mat1._row;
int col=mat1._col;
int col2=mat2._col;
for(int i=0;i<row;++i){
   for(int j=0;j<col2;++j){
      T d=0;
      for(int k=0;k<col;++k){
         d+=mat1[i][k]*mat2[k][j];
      }
      res[i][j]=d;
   }
}
}

template <class T> inline void mul_add(CDenseMatrix<T>& mat1,CDenseMatrix<T>& mat2,CDenseMatrix<T>& res){
int row=mat1._row;
int col=mat1._col;
int col2=mat2._col;
for(int i=0;i<row;++i){
   for(int j=0;j<col2;++j){
      T d=0;
      for(int k=0;k<col;++k){
         d+=mat1[i][k]*mat2[k][j];
      }
      res[i][j]+=d;
   }
}
}



template <class T> inline void mul(CDenseMatrix<T>& mat,CVector<T>& diag,CDenseMatrix<T>& res){
const int row=mat._row;
const int col=diag._size;
for(int i=0;i<row;++i){
   for(int j=0;j<col;++j){
      res[i][j]=mat[i][j]*diag[j];
   }
}
}



template <class T> inline void mulATA(CDenseMatrix<T>& mat,CDenseMatrix<T>& res){
int row=mat._row;
int col=mat._col;
for(int i=0;i<col;++i){
   for(int j=0;j<col;++j){
      T d=0;
      for(int k=0;k<row;++k){
         d+=mat[k][i]*mat[k][j];
      }
      res[i][j]=d;
   }
}
}

template <class T> inline void mul_nt(CDenseMatrix<T>& mat1,CDenseMatrix<T>& mat2,CDenseMatrix<T>& res){
int row=mat1._row;
int col=mat1._col;
int col2=mat2._row;
for(int i=0;i<row;++i){
   for(int j=0;j<col2;++j){
      T d=0;
      for(int k=0;k<col;++k){
         d+=mat1[i][k]*mat2[j][k];
      }
      res[i][j]=d;
   }
}
}

template <class T> inline void mul_tn(CDenseMatrix<T>& mat1,CDenseMatrix<T>& mat2,CDenseMatrix<T>& res){
int row=mat1._row;
int col=mat1._col;
int col2=mat2._col;
for(int i=0;i<col;++i){
   for(int j=0;j<col2;++j){
      T d=0;
      for(int k=0;k<row;++k){
         d+=mat1[k][i]*mat2[k][j];
      }
      res[i][j]=d;
   }
}
}


template <class T> inline void mul_nt_add(CDenseMatrix<T>& mat1,CDenseMatrix<T>& mat2,CDenseMatrix<T>& res){
int row=mat1._row;
int col=mat1._col;
int col2=mat2._row;
for(int i=0;i<row;++i){
   for(int j=0;j<col2;++j){
      T d=0;
      for(int k=0;k<col;++k){
         d+=mat1[i][k]*mat2[j][k];
      }
      res[i][j]+=d;
   }
}
}

template <class T> inline void mul_tn_add(CDenseMatrix<T>& mat1,CDenseMatrix<T>& mat2,CDenseMatrix<T>& res){
int row=mat1._row;
int col=mat1._col;
int col2=mat2._col;
for(int i=0;i<col;++i){
   for(int j=0;j<col2;++j){
      T d=0;
      for(int k=0;k<row;++k){
         d+=mat1[k][i]*mat2[k][j];
      }
      res[i][j]+=d;
   }
}
}




template <class T> inline void mul_tt(CDenseMatrix<T>& mat1,CDenseMatrix<T>& mat2,CDenseMatrix<T>& res){
int row=mat1._row;
int col=mat1._col;
int row2=mat2._row;
for(int i=0;i<col;++i){
   for(int j=0;j<row2;++j){
      T d=0;
      for(int k=0;k<row;++k){
         d+=mat1[k][i]*mat2[j][k];
      }
      res[i][j]=d;
   }
}
}

template <class T> inline void mul(CDenseMatrix<T>* a,CDenseMatrix<T>* b,CDenseMatrix<T>* c,CDenseMatrix<T>* res){
int aw=a->_col;
int ah=a->_row;
int bw=b->_col;
int cw=c->_col;
T** av=a->_value;
T** bv=b->_value;
T** cv=c->_value;
T** rv=res->_value;
T sum;

res->_zero();

for(int k=0;k<ah;++k){
   for(int j=0;j<bw;++j){
      sum=0.0;
      for(int i=0;i<aw;++i){
         sum+=av[k][i]*bv[i][j];
      }

      for(int i=0;i<cw;++i){
         rv[k][i]+=sum*cv[j][i];
      }
   }
}

}


template <class T> inline int inverse2x2(CDenseMatrix<T>& mat,CDenseMatrix<T>& res){
T det=mat(0,0)*mat(1,1)-mat(1,0)*mat(0,1);
if(det==0) return(-1);
det=1.0/det;
res(0,0)=det*mat(1,1);
res(0,1)=-det*mat(0,1);
res(1,0)=-det*mat(1,0);
res(1,1)=det*mat(0,0);
return(0);
}

template <class T> bool isEqual(CDenseMatrix<T>& A,CDenseMatrix<T>& B,T eps){
if(A._row!=B._row) return(false);
if(A._col!=B._col) return(false);
if(A._row!=A._col) return(false);
for(int i=0;i<A._row;++i){
   for(int j=0;j<A._col;++j){
      T diff=A[i][j]-B[i][j];
      if(diff>eps){
        std::cout<<"diff: "<<diff<<std::endl;
        return(false);
      }
   }
}
return(true);
}


template <class T> inline void calcUTAU(CDenseMatrix<T>& U,CDenseMatrix<T>& ASym,CDenseMatrix<T>& res){

res=0;

int col=U._col;
int col2=ASym._col;

for(int c=0;c<col2;++c){
   for(int r=0;r<c;++r){
      for(int c1=0;c1<col;++c1){
         for(int c2=0;c2<=c1;++c2){
            res[c1][c2]+=2*U[c][c2]*U[r][c1]*ASym[r][c];
         }
      }
   }
}

for(int c=0;c<col2;++c){
   for(int c1=0;c1<col;++c1){
      for(int c2=0;c2<=c1;++c2){
         res[c1][c2]+=U[c][c2]*U[c][c1]*ASym[c][c];
      }
   }
}

for(int c1=0;c1<col;++c1){
   for(int c2=c1+1;c2<col;++c2){
      res[c1][c2]=res[c2][c1];
   }
}

}



template <class T> inline void calcUTAQ(CDenseMatrix<T>& U,CDenseMatrix<T>& Q,CDenseMatrix<T>& ASym,CDenseMatrix<T>& res){

res=0;

const int urow=U._row;
const int ucol=U._col;
const int qcol=Q._col;
const int acol=ASym._col;

CDenseMatrix<T> tmp(ucol,acol);

for(int c1=0;c1<ucol;++c1){
   for(int c=0;c<acol;++c){
      T s=0;
      for(int r=0;r<urow;++r){
         s+=U[r][c1]*ASym[r][c];
      }
      tmp[c1][c]=s;
   }
}

for(int r=0;r<ucol;++r){
   for(int c1=0;c1<qcol;++c1){
      T s=0;
      for(int c=0;c<acol;++c){
         s+=tmp[r][c]*Q[c][c1];
      }
      res[r][c1]=s;
   }
}

}


template <class T> void calcUAV(CDenseMatrix<T>& U,CDenseMatrix<T>& A,CDenseMatrix<T>& V,CDenseMatrix<T>& R){

CVector<T> tmp(A._col);
for(int i=0;i<U._row;++i){
   for(int j=0;j<A._col;++j){
      T d=0;
      for(int k=0;k<U._col;++k){
         d+=U[i][k]*A[k][j];
      }
      tmp[j]=d;
   }
   
   for(int j=0;j<V._col;++j){
      T d=0;
      for(int k=0;k<A._col;++k){
         d+=tmp[k]*V[k][j];
      }
      R[i][j]=d;
   }
}

}


template <class T> void calcUTAU(CDenseMatrix<T>& U,CVector<T>& A,CDenseMatrix<T>& R){

CVector<T> tmp(A._size);
for(int j=0;j<U._col;++j){
   for(int i=0;i<A._size;++i){
      tmp[i]=U[i][j]*A[i];
   }
   for(int i=0;i<U._col;++i){
      T d=0;
      for(int k=0;k<A._size;++k) d+=tmp[k]*U[k][i];
      R[j][i]=d;
   }
}


}



template <class T> void sampleTwoGaussian(T& f1,T& f2){

double v1=((double)rand()+1)/((double)RAND_MAX+2.0);
double v2=((double)rand()+1)/((double)RAND_MAX+2.0);
double len=sqrt(-2*log(v1));
f1=len*cos(2*M_PI*v2);
f2=len*sin(2*M_PI*v2);

}

template <class T> void sampleGaussianMat(CDenseMatrix<T>& mat){

for(unsigned int i=0;i<mat._row;++i){
   unsigned int j=0;
   for(;j+1<mat._col;j+=2){
      T f1,f2;
      sampleTwoGaussian<T>(f1,f2);
      mat[i][j]=f1;
      mat[i][j+1]=f2;
   }
   for(;j<mat._col;++j){
      T f1,f2;
      sampleTwoGaussian<T>(f1,f2);
      mat[i][j]=f1;
   }
}

}



template <class T> void processGramSchmidt(CDenseMatrix<T>& mat,T eps=1.0e-2){

for(unsigned int i=0;i<mat._col;++i){
   for(unsigned int j=0;j<i;++j){
      T r=0;
      for(unsigned int l=0;l<mat._row;++l) r+=mat[l][i]*mat[l][j];
      T norm=0;
      for(unsigned int l=0;l<mat._row;++l){
         mat[l][i]-=r*mat[l][j];
         norm+=mat[l][i]*mat[l][i];
      }
      if(norm<eps){
        norm=1/sqrt(norm);
        for(unsigned int l=0;l<mat._row;++l) mat[l][i]*=norm;
      }
   }
   
   T norm=0;
   for(unsigned int l=0;l<mat._row;++l) norm+=square(mat[l][i]);
   norm=1/sqrt(norm);
   for(unsigned int l=0;l<mat._row;++l) mat[l][i]*=norm;
}

}


template <class T> void inverse(const CDenseMatrix<T>& L,CDenseMatrix<T>& Inv){

const int row=L._row;
const int col=L._col;

for(int c=0;c<col;++c){

   for(int i=0;i<row;++i){
      T s=(i==c)?1:0;
      for(int j=0;j<i;++j) s-=L[i][j]*Inv[j][c];
      Inv[i][c]=s/L[i][i];
   }

   for(int i=row-1;i>=0;--i){
      T s=Inv[i][c];
      for(int j=row-1;j>i;--j) s-=L[j][i]*Inv[j][c];
      Inv[i][c]=s/L[i][i];
   }
   
}

}



template <class T> void inverse(const CDenseMatrix<T>& LU,CVector<int>& order,CDenseMatrix<T>& Inv){

const int row=LU._row;
const int col=LU._col;

for(int c=0;c<col;++c){

   for(int i=0;i<row;++i){
      T s=(i==c)?1:0;
      for(int j=0;j<i;++j) s-=LU[order[i]][j]*Inv[j][c];
      Inv[i][c]=s;
   }

   for(int i=row-1;i>=0;--i){
      T s=Inv[i][c];
      for(int j=row-1;j>i;--j) s-=LU[order[i]][j]*Inv[j][c];
      Inv[i][c]=s/LU[order[i]][i];
   }
}

}


template <class T> T quadratic(CVector<T>& u,CDenseMatrix<T>& A){
 
T s=0;

for(int j=0;j<A._col;++j){
   T d=0;
   for(int i=0;i<u._size;++i){
      d+=u[i]*A[i][j];
   }
   s*=d*u[j];
}


}


template <class T> void makeHadamardMatrix(const int n,CDenseMatrix<T>& H){

H[0][0]=1;
int r=1;
for(int i=1;i<n;++i){
   
   for(int j=0;j<r;++j){
      for(int k=0;k<r;++k){
         H[j][k+r]=H[j][k];
         H[j+r][k]=H[j][k];
         H[j+r][k+r]=-H[j][k];
      }
   }
      
   r>>=1;
}

}

template <class T> void tensorProduct(CVector<T>& x,CVector<T>& y,CDenseMatrix<T>& A){

for(int i=0;i<x._size;++i){
   for(int j=0;j<y._size;++j) A[i][j]=x[i]*y[j];
}

}


template <class T> void tensorProductAdd(CVector<T>& x,CVector<T>& y,CDenseMatrix<T>& A){
for(int i=0;i<x._size;++i){
   for(int j=0;j<y._size;++j) A[i][j]+=x[i]*y[j];
}
}


template <class T> void tensorProduct(CVector<T>& x,CDenseMatrix<T>& A){
for(int i=0;i<x._size;++i){
   for(int j=0;j<x._size;++j) A[i][j]=x[i]*x[j];
}
}


template <class T> void LUdecomp(const CDenseMatrix<T>& A,CDenseMatrix<T>& LU,CVector<int>& order){

const int row=A._row;
const int col=A._col;

LU=A;
for(int i=0;i<row;++i) order[i]=i;

for(int i=0;i<row-1;++i){

   int d=i;
   T maxd=square(LU[i][i]);
   for(int j=i+1;j<row;++j){
      if(maxd<square(LU[j][i])){
        d=j;
        maxd=square(LU[j][i]);
      }
   }

   if(d!=i){
     int tmp=order[i];
     order[i]=order[d];
     order[d]=tmp;

     for(int j=0;j<col;++j){
        T t=LU[i][j];
        LU[i][j]=LU[d][j];
        LU[d][j]=t;
     }
   }
   
   T v=1.0/LU[i][i];
   if(v!=v) continue;
   
   for(int j=i+1;j<row;++j){
      LU[j][i]*=v;
      for(int k=i+1;k<col;++k){
         LU[j][k]-=LU[j][i]*LU[i][k];
      }
   }
   
}


}


// solve L*U * x = o(b)
template <class T> void SolveDirect(const CDenseMatrix<T>& LU,T* x,const T* b,CVector<int>& order){

const int row=LU._row;
for(int i=0;i<row;++i) x[i]=b[order[i]];



for(int i=0;i<row;++i){
   for(int j=0;j<i;++j) x[i]-=LU[i][j]*x[j];
}

for(int i=row-1;i>=0;--i){
   for(int j=row-1;j>i;--j) x[i]-=LU[i][j]*x[j];
   x[i]/=LU[i][i];
}

}



#endif
