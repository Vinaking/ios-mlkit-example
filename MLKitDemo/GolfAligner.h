//
//  GolfAligner.h
//  MLKitDemo
//
//  Created by Tùng on 31/10/2022.
//

//#ifndef GolfAligner_h
//#define GolfAligner_h
//
//
//#endif /* GolfAligner_h */



#ifndef GolfAligner_h
#define GolfAligner_h

#include "CDenseMatrix.h"

typedef struct CVector3d{
float _x=0,_y=0,_z=0;
} CVector3d;

typedef struct Skeleton{
float _visibility=1.0f;
CVector3d _origin;
CVector3d _pos[33];
} Skeleton;

typedef struct StrikeData{
std::string _name;
int _nShots=0;
std::vector<float> _add,_top,_imp,_fin;
std::vector<bool> _hasImpact;
int _width=0,_height=0;
float _fps=0;
float _scale=1;
} StrikeData;


class GolfMotionNode;

class GolfMotionAligner{

private:

GolfMotionNode* _dendrogram=nullptr;
std::vector<std::string> _files;
CArray<std::vector<CPair<int,int>>> _path;
CDenseMatrix<float> _D;

public:

virtual ~GolfMotionAligner(){ if(_dendrogram) delete _dendrogram; }
void _load(std::string dendrogram);
void _setDatabase(std::string dir,std::vector<StrikeData>& dataset);
void _train(std::vector<StrikeData>& dataset,std::string dendrogram);
//const int _traverse(std::string npz);
const int _getNumShots(){ return(_path._size); }
const std::string _getFilename(const int idx){ return(_files[idx]); }
const std::vector<CPair<int,int>>& _getAlignment(const int idx){ return(_path[idx]); }

};


#endif
