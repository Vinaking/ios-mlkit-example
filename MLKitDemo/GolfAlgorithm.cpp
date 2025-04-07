//
//  GolfAlgorithm.cpp
//  MLKitDemo
//
//  Created by Tùng on 31/10/2022.
//

//#include "GolfAlgorithm.hpp"


#include "GolfAligner.h"


void subsequence_cost_matrix(CDenseMatrix<float>& S,CDenseMatrix<float>& L,CDenseMatrix<float>& C){

const int len1=S._row;
const int len2=L._row;

CDenseMatrix<float> Cost(len1+1,len2+1);
Cost=FLT_MAX;
for(int i=0;i<Cost._col;++i) Cost[0][i]=0;

for(int i=0;i<len1;++i){
   for(int j=0;j<len2;++j){
      float d=0;
      for(int t=0;t<S._col;++t) d+=square(S[i][t]-L[j][t]);
      Cost[i+1][j+1]=d;
      Cost[i+1][j+1]+=fmin(fmin(Cost[i][j+1],Cost[i+1][j]),Cost[i][j]);
   }
}

for(int i=0;i<len1;++i){
   for(int j=0;j<len2;++j){
      C[i][j]=Cost[i+1][j+1];
   }
}

}


void subsequence_path(CDenseMatrix<float>& Cost,size_t pathstart,size_t pathend,std::vector<int>& path1,std::vector<int>& path2){

path1.push_back(Cost._row-1);
path2.push_back(pathend);

while(path1[path1.size()-1]!=0){
     int i=path1[path1.size()-1];
     int j=path2[path2.size()-1];
     if(i==0){
       path1.push_back(0); path2.push_back(j-1);
     }else if(j==0){
       path1.push_back(i-1); path2.push_back(0);
     }else{
       if(Cost[i-1][j-1]<Cost[i-1][j] && Cost[i-1][j-1]<Cost[i][j-1]){
         path1.push_back(i-1); path2.push_back(j-1);
       }else if(Cost[i-1][j]<Cost[i-1][j-1] && Cost[i-1][j]<Cost[i][j-1]){
         path1.push_back(i-1); path2.push_back(j);
       }else{
         path1.push_back(i); path2.push_back(j-1);
       }
     }
}

}


void local_maxima_1d(CVector<float>& x,std::vector<int>& peaks){

int i=1;
while(i<x._size-1){
     if(x[i-1]<x[i]){
       int j=i+1;
       while(j<x._size){
            if(x[j]!=x[i]) break;
            ++j;
       }
       if(x[j]<x[i]){
         peaks.push_back((i+j-1)/2);
         i=j;
       }
     }
     ++i;
}

}
 

template <class T> std::vector<size_t> argsort(const std::vector<T>& array){
std::vector<size_t> indices(array.size());
std::iota(indices.begin(),indices.end(),0);
std::sort(indices.begin(),indices.end(),
    [&array](int left,int right) -> bool {
        return(array[left]<array[right]);
    }
);
return(indices);
}



const float motionAlignment(const int finPos,CDenseMatrix<float>& S,CDenseMatrix<float>& L,CArray<std::vector<CPair<int,int>>>& path){

CDenseMatrix<float> C(S._row,L._row);
subsequence_cost_matrix(S,L,C);
CVector<float> costfunc(L._row);
for(int i=0;i<L._row;++i) costfunc[i]=-C[finPos][i];
std::vector<int> peaks;
local_maxima_1d(costfunc,peaks);

std::cout<<"([";
for(int i=0;i<costfunc._size;++i) std::cout<<costfunc[i]<<",";
std::cout<<"])"<<std::endl<<std::endl<<std::endl;

std::vector<float> cval(peaks.size());
for(int i=0;i<cval.size();++i) cval[i]=costfunc[peaks[i]];
auto indices=argsort(cval);
CVector<char> keeps(peaks.size());
keeps=1;

for(int i=0;i<peaks.size();++i){
   if(costfunc[peaks[i]]<-1000) keeps[i]=0;
}

int mindist=S._row*0.5;
for(int i=peaks.size()-1;i>=0;--i){
   int j=indices[i];
   if(keeps[j]==0) continue;;
   int k=j-1;
   while(k>=0){
        if(peaks[j]-peaks[k]>mindist) break;
        keeps[k]=0;
        --k;
   }
   k=j+1;
   while(k<peaks.size()){
        if(peaks[k]-peaks[j]>mindist) break;
        keeps[k]=0;
        ++k;
   }
}

std::vector<int> results;
int n=0;
for(int i=0;i<peaks.size();++i){
   if(keeps[indices[i]]==0) continue;
   results.push_back(peaks[indices[i]]);
   ++n;
}
CArray<std::vector<CPair<int,int>>> tmp;
tmp._resize(n);
CVector<float> pathcost(n);

n=0;
for(int i=0;i<results.size();++i){
   std::vector<int> p1,p2;
   subsequence_path(C,(i==0)?0:results[i-1],results[i],p1,p2);
   std::vector<CPair<int,int>> l;
   for(int j=p1.size()-1;j>=0;--j){
      CPair<int,int> p(p1[j],p2[j]);
      l.push_back(p);
   }
   const int begin=l[0]._second;
   const int end=l[l.size()-1]._second;
   bool overlaped=false;
   for(int j=0;j<n;++j){
      const int b=tmp[j][0]._second;
      const int e=tmp[j][tmp[j].size()-1]._second;
      if(!(e<begin || end<b)){
        std::vector<CPair<int,int>> r;
        int k=0;
        while(tmp[j][k]._second<begin){
             r.push_back(tmp[j][k]);
             ++k;
             if(k>=tmp[j].size()) break;
        }
        while(tmp[j][k]._second<=end){
             ++k;
             if(k>=tmp[j].size()) break;
        }
        for(int kk=0;kk<l.size();++kk) r.push_back(l[kk]);
        for(;k<tmp[j].size();++k) r.push_back(tmp[j][k]);
        tmp[j]=r;
        pathcost[j]=costfunc[results[i]]/(float)r.size();
        overlaped=true;
        //for(int t=0;t<r.size();++t) std::cout<<"over: "<<r[t]._first<<" "<<r[t]._second<<std::endl;
        break;
      }
   }
   if(!overlaped){
     tmp[n]=l;
     pathcost[n]=costfunc[results[i]]/(float)l.size();
     //for(int t=0;t<l.size();++t) std::cout<<"new: "<<l[t]._first<<" "<<l[t]._second<<std::endl;
     ++n;
   }
}

float cost=0;
path._resize(n);
for(int i=0;i<n;++i){
   path[i]=tmp[i];
   cost+=pathcost[i];
}

if(n==0){
  std::cout<<"ERROR: no shot found!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
  return(FLT_MAX);
}else{
  std::cout<<n<<" shots found "<<cost/(float)n<<std::endl;
}

cost/=(float)n;

return(cost);
}


template <class T> void TimeSeriesScalerMeanVariance(CDenseMatrix<T>& A,const T mu=0,const T std=1){

/*for(int j=0;j<A._row;++j){
   T m=0;
   for(int i=0;i<A._col;++i) m+=A[j][i];
   m/=(T)A._col;
   T s=0;
   for(int i=0;i<A._col;++i) s+=square(A[j][i]-m);
   s/=(T)A._col;
   if(s==0) s=1;
   for(int i=0;i<A._col;++i) A[j][i]=(A[j][i]-m)*s/std+mu;
}*/

for(int j=0;j<A._col;++j){
   T m=0;
   for(int i=0;i<A._row;++i) m+=A[i][j];
   m/=(T)A._row;
   T s=0;
   for(int i=0;i<A._row;++i) s+=square(A[i][j]-m);
   s/=(T)A._row;
   if(s==0) s=1;
   for(int i=0;i<A._row;++i) A[i][j]=(A[i][j]-m)*s/std+mu;
}

}




class GolfMotionNode{

public:

GolfMotionNode *_left=nullptr,*_right=nullptr;
int _div=0;
int _size=0;
int _candidate=0;
std::vector<int> _indices;


virtual ~GolfMotionNode(){

if(_left) delete _left;
if(_right) delete _right;

}


void _build(CDenseMatrix<float>& D){

float mind=FLT_MAX;
int mini=0;
for(int i=0;i<_indices.size();++i){
   float d=0;
   for(int j=0;j<_indices.size();++j){
      if(i==j) continue;;
      d+=D[_indices[i]][_indices[j]];
   }
   if(d<mind){
     mind=d;
     mini=i;
   }
}
_candidate=mini;

}
/*

const int _traverse(std::vector<std::string>& files,CDenseMatrix<float>& target,CVector<float>& D){

D=-1;

float mind=FLT_MAX;
GolfMotionNode* node=nullptr;

if(_left){
  if(D[_indices[_left->_candidate]]<0){
     std::map<std::string,std::vector<int>> shapes;
     std::map<std::string,CVector<unsigned char>> data;
     loadNPZ(files[_indices[_left->_candidate]],shapes,data);
     CVector<float> ref;
     move(data["trajectory"],ref);
     const int len=shapes["trajectory"][0];
     CDenseMatrix<float> R(len,3*33);
     for(int t=0;t<len;++t){
         Skeleton* s=(Skeleton*)&ref[(3*33+3+1)*t];
         for(int m=0;m<33;++m){
            R[t][3*m]=s->_pos[m]._x;
            R[t][3*m+1]=s->_pos[m]._y;
            R[t][3*m+2]=s->_pos[m]._z;
          }
     }
     TimeSeriesScalerMeanVariance(R);
     CArray<std::vector<CPair<int,int>>> path;
     D[_indices[_left->_candidate]]=motionAlignment(R,target,path);
  }
  if(D[_indices[_left->_candidate]]<mind){
    mind=D[_indices[_left->_candidate]];
    node=_left;
  }
}

if(_right){
  if(D[_indices[_div+_right->_candidate]]<0){
     std::map<std::string,std::vector<int>> shapes;
     std::map<std::string,CVector<unsigned char>> data;
     loadNPZ(files[_indices[_div+_right->_candidate]],shapes,data);
     CVector<float> ref;
     move(data["trajectory"],ref);
     const int len=shapes["trajectory"][0];
     CDenseMatrix<float> R(len,3*33);
     for(int t=0;t<len;++t){
         Skeleton* s=(Skeleton*)&ref[(3*33+3+1)*t];
         for(int m=0;m<33;++m){
            R[t][3*m]=s->_pos[m]._x;
            R[t][3*m+1]=s->_pos[m]._y;
            R[t][3*m+2]=s->_pos[m]._z;
          }
     }
     TimeSeriesScalerMeanVariance(R);
     CArray<std::vector<CPair<int,int>>> path;
     D[_indices[_div+_right->_candidate]]=motionAlignment(R,target,path);
  }
  if(D[_indices[_div+_right->_candidate]]<mind){
    mind=D[_indices[_div+_right->_candidate]];
    node=_right;
  }
}

if(node) return(node->_traverse(&_indices[(node==_left)?0:_div],files,target,D));

return(_indices[_candidate]);
}


const int _traverse(int* indices,std::vector<std::string>& files,CDenseMatrix<float>& target,CVector<float>& D){

float mind=FLT_MAX;
GolfMotionNode* node=nullptr;

if(_left){
  if(D[indices[_left->_candidate]]<0){
     std::map<std::string,std::vector<int>> shapes;
     std::map<std::string,CVector<unsigned char>> data;
     loadNPZ(files[indices[_left->_candidate]],shapes,data);
     CVector<float> ref;
     move(data["trajectory"],ref);
     const int len=shapes["trajectory"][0];
     CDenseMatrix<float> R(len,3*33);
     for(int t=0;t<len;++t){
         Skeleton* s=(Skeleton*)&ref[(3*33+3+1)*t];
         for(int m=0;m<33;++m){
            R[t][3*m]=s->_pos[m]._x;
            R[t][3*m+1]=s->_pos[m]._y;
            R[t][3*m+2]=s->_pos[m]._z;
          }
     }
     TimeSeriesScalerMeanVariance(R);
     CArray<std::vector<CPair<int,int>>> path;
     D[indices[_left->_candidate]]=motionAlignment(R,target,path);
  }
  if(D[indices[_left->_candidate]]<mind){
    mind=D[_indices[_left->_candidate]];
    node=_left;
  }
}

if(_right){
  if(D[indices[_div+_right->_candidate]]<0){
     std::map<std::string,std::vector<int>> shapes;
     std::map<std::string,CVector<unsigned char>> data;
     loadNPZ(files[indices[_div+_right->_candidate]],shapes,data);
     CVector<float> ref;
     move(data["trajectory"],ref);
     const int len=shapes["trajectory"][0];
     CDenseMatrix<float> R(len,3*33);
     for(int t=0;t<len;++t){
         Skeleton* s=(Skeleton*)&ref[(3*33+3+1)*t];
         for(int m=0;m<33;++m){
            R[t][3*m]=s->_pos[m]._x;
            R[t][3*m+1]=s->_pos[m]._y;
            R[t][3*m+2]=s->_pos[m]._z;
          }
     }
     TimeSeriesScalerMeanVariance(R);
     CArray<std::vector<CPair<int,int>>> path;
     D[indices[_div+_right->_candidate]]=motionAlignment(R,target,path);
  }
  if(D[indices[_div+_right->_candidate]]<mind){
    mind=D[indices[_div+_right->_candidate]];
    node=_right;
  }
}

if(node) return(node->_traverse(&_indices[(node==_left)?0:_div],files,target,D));

return(_indices[_candidate]);
}

*/
void _save(std::ofstream& ofs,const bool root=false){

ofs.write((char*)&_div,sizeof(int));
ofs.write((char*)&_size,sizeof(int));
ofs.write((char*)&_candidate,sizeof(int));
if(root){
  int size=_indices.size();
  ofs.write((char*)&size,sizeof(int));
  ofs.write((char*)&_indices[0],size*sizeof(int));
}

int left=0;
if(_left){
  left=1;
  ofs.write((char*)&left,sizeof(int));
  _left->_save(ofs);
}else ofs.write((char*)&left,sizeof(int));

int right=0;
if(_right){
  right=1;
  ofs.write((char*)&right,sizeof(int));
  _right->_save(ofs);
}else ofs.write((char*)&right,sizeof(int));

}


void _load(std::ifstream& ifs,const bool root=false){

ifs.read((char*)&_div,sizeof(int));
ifs.read((char*)&_size,sizeof(int));
ifs.read((char*)&_candidate,sizeof(int));
if(root){
  int size=0;
  ifs.read((char*)&size,sizeof(int));
  _indices.resize(size);
  ifs.read((char*)&_indices[0],size*sizeof(int));
}

int left=0;
ifs.read((char*)&left,sizeof(int));
if(left){
  _left=new GolfMotionNode;
  _left->_load(ifs);
}

int right=0;
ifs.read((char*)&right,sizeof(int));
if(right){
  _right=new GolfMotionNode;
  _right->_load(ifs);
}

}


};


const float dist(CDenseMatrix<float>& D,GolfMotionNode* n1,GolfMotionNode* n2){

float d=0;
int n=0;
for(int j=0;j<n1->_indices.size();++j){
   for(int i=0;i<n2->_indices.size();++i){
      d+=D[n1->_indices[j]][n2->_indices[i]];
      ++n;
   }
}

return(d/(float)n);
}


void GolfMotionAligner::_load(std::string dendrogram){

std::ifstream ifs(dendrogram);
_D._load(ifs);
_dendrogram=new GolfMotionNode;
_dendrogram->_load(ifs,true);

}


void GolfMotionAligner::_setDatabase(std::string dir,std::vector<StrikeData>& dataset){

for(int i=0;i<dataset.size();++i){
   std::string path=dir;
   path.append(dataset[i]._name);
   path.append(".npz");
   if(std::filesystem::exists(path)){
     _files.push_back(path);
     std::cout<<path<<std::endl;
   }
}

}

/*
const int GolfMotionAligner::_traverse(std::string npz){

std::map<std::string,std::vector<int>> shape;
std::map<std::string,CVector<unsigned char>> data;
loadNPZ(npz,shape,data);
CVector<float> tmp;
move(data["trajectory"],tmp);
int len=shape["trajectory"][0];
CDenseMatrix<float> userMotion(len,3*33);
for(int t=0;t<len;++t){
   Skeleton* s=(Skeleton*)&tmp[(3*33+3+1)*t];
   for(int m=0;m<33;++m){
      userMotion[t][3*m]=s->_pos[m]._x;
      userMotion[t][3*m+1]=s->_pos[m]._y;
      userMotion[t][3*m+2]=s->_pos[m]._z;
   }
}
TimeSeriesScalerMeanVariance(userMotion);

CVector<float> D(_files.size());
const int ref=_dendrogram->_traverse(_files,userMotion,D);

loadNPZ(_files[ref],shape,data);
move(data["trajectory"],tmp);
len=shape["trajectory"][0];
CDenseMatrix<float> Ref(len,3*33);
for(int t=0;t<len;++t){
   Skeleton* s=(Skeleton*)&tmp[(3*33+3+1)*t];
   for(int m=0;m<33;++m){
      Ref[t][3*m]=s->_pos[m]._x;
      Ref[t][3*m+1]=s->_pos[m]._y;
      Ref[t][3*m+2]=s->_pos[m]._z;
   }
}
TimeSeriesScalerMeanVariance(Ref);
motionAlignment(Ref,userMotion,_path);

return(ref);
}*/



void GolfMotionAligner::_train(std::vector<StrikeData>& dataset,std::string dendrogram){

_D._resize(_files.size(),_files.size());
_D=0.0f;

for(int j=0;j<_files.size();++j){
   std::map<std::string,std::vector<int>> shapes1;
   std::map<std::string,CVector<unsigned char>> data1;
   loadNPZ(_files[j],shapes1,data1);
   CVector<float> t1;
   move(data1["trajectory"],t1);
   CVector<float> s1;
   move(data1["scale"],s1);
   const float scale1=s1[0];
   move(data1["fps"],s1);
   const float fps1=29.97;//s1[0];
   const float fin1=dataset[j]._fin[0];
   const int len1=shapes1["trajectory"][0];
   CDenseMatrix<float> S1(len1,3*33);
   
   std::cout<<len1<<" "<<fps1<<" "<<fin1<<" "<<fps1*fin1<<std::endl;
   
   for(int t=0;t<len1;++t){
      Skeleton* s=(Skeleton*)&t1[(3*33+3+1)*t];
      if(s->_visibility<0){
        bool find=false;
        for(int y=t+1;y<len1;++y){
           Skeleton* ss=(Skeleton*)&t1[(3*33+3+1)*y];
           if(ss->_visibility>0){
             *s=*ss;
             find=true;
             break;
           }
        }
        if(!find){
          for(int y=t-1;y>=0;--y){
           Skeleton* ss=(Skeleton*)&t1[(3*33+3+1)*y];
           if(ss->_visibility>0){
             *s=*ss;
             break;
           }
        }
        }
      }
    }
   
   for(int t=0;t<len1;++t){
      Skeleton* s=(Skeleton*)&t1[(3*33+3+1)*t];
      for(int m=0;m<33;++m){
         S1[t][3*m]=s->_pos[m]._x/scale1;
         S1[t][3*m+1]=0;//s->_pos[m]._y/s1[0];
         S1[t][3*m+2]=s->_pos[m]._z/scale1;
      }
   }
   //TimeSeriesScalerMeanVariance(S1);
   for(int i=j+1;i<_files.size();++i){
      std::map<std::string,std::vector<int>> shapes2;
      std::map<std::string,CVector<unsigned char>> data2;
      loadNPZ(_files[i],shapes2,data2);
      CVector<float> t2;
      move(data2["trajectory"],t2);
      const int len2=shapes2["trajectory"][0];
      CVector<float> s2;
      move(data2["scale"],s2);
      CDenseMatrix<float> S2(2*len1,3*33);
      
      for(int t=0;t<len2;++t){
      Skeleton* s=(Skeleton*)&t2[(3*33+3+1)*t];
      if(s->_visibility<0){
        bool find=false;
        for(int y=t+1;y<len2;++y){
           Skeleton* ss=(Skeleton*)&t2[(3*33+3+1)*y];
           if(ss->_visibility>0){
             *s=*ss;
             find=true;
             break;
           }
        }
        if(!find){
          for(int y=t-1;y>=0;--y){
           Skeleton* ss=(Skeleton*)&t2[(3*33+3+1)*y];
           if(ss->_visibility>0){
             *s=*ss;
             break;
           }
        }
        }
      }
    }
    
    
      for(int t=0;t<len1;++t){
         Skeleton* s=(Skeleton*)&t1[(3*33+3+1)*t];
         for(int m=0;m<33;++m){
            S2[t][3*m]=s->_pos[m]._x/scale1;
            S2[t][3*m+1]=0;//s->_pos[m]._y/s1[0];
            S2[t][3*m+2]=s->_pos[m]._z/scale1;
         }
      }
      for(int t=0;t<len1;++t){
         Skeleton* s=(Skeleton*)&t1[(3*33+3+1)*t];
         for(int m=0;m<33;++m){
            S2[len1+t][3*m]=s->_pos[m]._x/scale1;
            S2[len1+t][3*m+1]=0;//s->_pos[m]._y/s1[0];
            S2[len1+t][3*m+2]=s->_pos[m]._z/scale1;
         }
      }
      //TimeSeriesScalerMeanVariance(S2);
      std::cout<<_files[j]<<" "<<_files[i]<<std::endl;
      CArray<std::vector<CPair<int,int>>> path;
      float d=(S1._row<=S2._row)?motionAlignment(fin1*fps1+0.5,S1,S2,path):motionAlignment(fin1*fps1+0.5,S2,S1,path);
      _D[j][i]=d;
      _D[i][j]=_D[j][i];
   }
}

return;
std::vector<GolfMotionNode*> nodes;
for(int i=0;i<_files.size();++i){
   GolfMotionNode* l=new GolfMotionNode;
   l->_size=1;
   l->_indices.resize(1);
   l->_indices[0]=i;
   nodes.push_back(l);
}

GolfMotionNode* root=nullptr;
bool updated=true;
while(updated){
     
     updated=false;
     
     float mind=FLT_MAX;
     int pair[2]={0,0};
     for(int j=0;j<nodes.size();++j){
        if(!nodes[j]) continue;
        for(int i=0;i<j;++i){
           if(!nodes[i]) continue;
           const float d=dist(_D,nodes[j],nodes[i]);
           if(d<mind){
             mind=d;
             pair[0]=j;
             pair[1]=i;
             updated=true;
           }
        }
     }
     
     if(updated){
       GolfMotionNode* node=new GolfMotionNode;
       node->_left=nodes[pair[0]];
       node->_right=nodes[pair[1]];
       node->_indices.reserve(node->_left->_indices.size()+node->_right->_indices.size());
       for(int i=0;i<node->_left->_indices.size();++i) node->_indices.push_back(node->_left->_indices[i]);
       node->_div=node->_left->_indices.size();
       for(int i=0;i<node->_right->_indices.size();++i) node->_indices.push_back(node->_right->_indices[i]);
       node->_size=node->_indices.size();
       node->_left->_indices.clear();
       node->_right->_indices.clear();
       node->_left->_indices.shrink_to_fit();
       node->_right->_indices.shrink_to_fit();
       node->_build(_D);
       nodes[pair[0]]=node;
       nodes[pair[1]]=nullptr;
       root=node;
     }
     
}

std::ofstream ofs(dendrogram);
_D._save(ofs);
root->_save(ofs,true);
delete root;

}


