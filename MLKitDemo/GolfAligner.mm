//
//  GolfAligner.m
//  MLKitDemo
//
//  Created by Tùng on 31/10/2022.
//

//#import <Foundation/Foundation.h>



#import "MLKitDemo-Bridging-Header.h"
#import <AVFoundation/AVFoundation.h>
#import <Accelerate/Accelerate.h>
#include "GolfAligner.h"
#include "OnsetDetector.h"

#define NDATASET 65


@implementation GolfAligner

std::string _libPath;
unsigned int _currentFrame=0;
unsigned int _resultFrame=0;
int _wavLength=0;
std::vector<float> _wav;
StrikeData _target,_ref;
CArray<Skeleton> _skeleton,_skeleton_ref;
std::vector<Skeleton> _trajectory;
PoseTracker* _tracker;
std::vector<StrikeData> _dataset;
GolfMotionAligner _aligner;


-(id)init{
    
    self=[super init];
    _tracker=[[PoseTracker alloc] init];
    [_tracker startGraph];
    _tracker.delegate=self;
    
    return self;
}


-(int)getNumDataset{
    return(_dataset.size());
}


-(NSString*)getDataset:(int)idx {
    return([NSString stringWithUTF8String:_dataset[idx]._name.c_str()]);
}


-(void)setLibraryPath:(NSString*)path {
    
    _libPath=[path UTF8String];
    _libPath.append("/");
    std::cout<<"Library path: "<<_libPath<<std::endl;
    
    NSString* csv=[[NSBundle mainBundle] pathForResource:@"dataset" ofType:@"csv"];
    std::ifstream ifs([csv UTF8String]);
    _dataset.clear();
    while(!ifs.eof()){
        StrikeData data;
        ifs>>data._name;
        ifs>>data._width;
        ifs>>data._height;
        ifs>>data._fps;
        ifs>>data._nShots;
        float tmp;
        for(int i=0;i<data._nShots;++i){
            ifs>>tmp;
            data._add.push_back(tmp);
            ifs>>tmp;
            data._top.push_back(tmp);
            ifs>>tmp;
            data._imp.push_back(tmp);
            ifs>>tmp;
            data._fin.push_back(tmp);
        }
        _dataset.push_back(data);
        if(_dataset.size()>=NDATASET) break;
    }
    ifs.close();
    
    
    std::string database=[[[NSBundle mainBundle] bundlePath] UTF8String];
    database.append("/database/");
    _aligner._setDatabase(database,_dataset);
    
    std::string tree=_libPath;
    tree.append("database.tree");
    //_aligner._train(_dataset,database);
    //_aligner._load(database);
    
}


-(int)getNumShots{
    return(_target._nShots);
}

-(float)getTopFrame:(int)idx {
    return(_target._top[idx]*_target._fps);
}

-(float)getAddFrame:(int)idx {
    return(_target._add[idx]*_target._fps);
}

-(float)getImpFrame:(int)idx {
    return(_target._imp[idx]*_target._fps);
}

-(bool)hasImpact:(int)idx; {
    return(_target._hasImpact[idx]);
}

-(float)getFinFrame:(int)idx {
    return(_target._fin[idx]*_target._fps);
}

-(int)getRefNumShots{
    return(_ref._nShots);
}

-(float)getRefTopFrame:(int)idx {
    return(_ref._top[idx]*_ref._fps);
}

-(float)getRefAddFrame:(int)idx {
    return(_ref._add[idx]*_ref._fps);
}

-(float)getRefImpFrame:(int)idx {
    return(_ref._imp[idx]*_ref._fps);
}

-(bool)hasRefImpact:(int)idx; {
    return(_ref._hasImpact[idx]);
}

-(float)getRefFinFrame:(int)idx {
    return(_ref._fin[idx]*_ref._fps);
}


-(bool)getSkeltonPos:(int)frame head_x:(int*)head_x head_y:(int*)head_y left_x:(int*)left_x left_y:(int*)left_y right_x:(int*)right_x right_y:(int*)right_y {
    
    *head_x=(_skeleton[frame]._pos[0]._x+_skeleton[frame]._origin._x)*_target._width;
    *head_y=-(_skeleton[frame]._pos[0]._z+_skeleton[frame]._origin._z)*_target._height;
    *left_x=(_skeleton[frame]._pos[19]._x+_skeleton[frame]._origin._x)*_target._width;
    *left_y=-(_skeleton[frame]._pos[19]._z+_skeleton[frame]._origin._z)*_target._height;
    *right_x=(_skeleton[frame]._pos[20]._x+_skeleton[frame]._origin._x)*_target._width;
    *right_y=-(_skeleton[frame]._pos[20]._z+_skeleton[frame]._origin._z)*_target._height;
    
    return(_skeleton[frame]._visibility>0);
}

-(bool)getRefSkeltonPos:(int)frame head_x:(int*)head_x head_y:(int*)head_y left_x:(int*)left_x left_y:(int*)left_y right_x:(int*)right_x right_y:(int*)right_y {
    
    *head_x=(_skeleton_ref[frame]._pos[0]._x+_skeleton_ref[frame]._origin._x)*_ref._width;
    *head_y=-(_skeleton_ref[frame]._pos[0]._z+_skeleton_ref[frame]._origin._z)*_ref._height;
    *left_x=(_skeleton_ref[frame]._pos[19]._x+_skeleton_ref[frame]._origin._x)*_ref._width;
    *left_y=-(_skeleton_ref[frame]._pos[19]._z+_skeleton_ref[frame]._origin._z)*_ref._height;
    *right_x=(_skeleton_ref[frame]._pos[20]._x+_skeleton_ref[frame]._origin._x)*_ref._width;
    *right_y=-(_skeleton_ref[frame]._pos[20]._z+_skeleton_ref[frame]._origin._z)*_ref._height;
    
    return(_skeleton_ref[frame]._visibility>0);
}


-(void)setTargetVideo:(NSString*)filename {
    
    _target._name=[filename UTF8String];
    
    NSString* csv=[[NSBundle mainBundle] pathForResource:@"dataset" ofType:@"csv"];
    std::ifstream ifs([csv UTF8String]);
    _target._add.clear(); _target._top.clear(); _target._imp.clear(); _target._fin.clear();
    _target._nShots=0;
    bool find=false;
    while(!ifs.eof()){
        StrikeData data;
        ifs>>data._name;
        ifs>>data._width;
        ifs>>data._height;
        ifs>>data._fps;
        ifs>>data._nShots;
        float tmp;
        for(int i=0;i<data._nShots;++i){
            ifs>>tmp;
            if(_target._name==data._name) _target._add.push_back(tmp);
            ifs>>tmp;
            if(_target._name==data._name) _target._top.push_back(tmp);
            ifs>>tmp;
            if(_target._name==data._name) _target._imp.push_back(tmp);
            ifs>>tmp;
            if(_target._name==data._name) _target._fin.push_back(tmp);
        }
        if(_target._name==data._name){
            find=true;
            _target._nShots=data._nShots;
            _target._width=data._width;
            _target._height=data._height;
            _target._fps=data._fps;
            break;
        }
    }
    
    
    
    if(find){
        [self load];
    }else{
        std::cout<<" New video must be preproccessed. "<<std::endl;
        exit(-1);
    }
    
}

-(void)get4Point {
//    std::cout<<"add:"<<_target._add<<" top:"<<_target._top<<" imp:"<<_target._top<<" fin:"<<_target._fin<<std::endl;
}

-(void)setRefVideo:(NSString*)filename {
    
    NSString* csv=[[NSBundle mainBundle] pathForResource:@"dataset" ofType:@"csv"];
    std::ifstream ifs([csv UTF8String]);
    
    _ref._add.clear(); _ref._top.clear(); _ref._imp.clear(); _ref._fin.clear();
    _ref._nShots=0;
    bool find=false;
    while(!ifs.eof()){
        StrikeData data;
        ifs>>data._name;
        ifs>>data._width;
        ifs>>data._height;
        ifs>>data._fps;
        ifs>>data._nShots;
        float tmp;
        for(int i=0;i<data._nShots;++i){
            ifs>>tmp;
            if(_ref._name==data._name) _ref._add.push_back(tmp);
            ifs>>tmp;
            if(_ref._name==data._name) _ref._top.push_back(tmp);
            ifs>>tmp;
            if(_ref._name==data._name) _ref._imp.push_back(tmp);
            ifs>>tmp;
            if(_ref._name==data._name) _ref._fin.push_back(tmp);
        }
        if(_ref._name==data._name){
            find=true;
            _ref._width=data._width;
            _ref._height=data._height;
            _ref._fps=data._fps;
            break;
        }
    }
    
    if(find){
        [self loadRef];
    }else{
        std::cout<<" New video must be preproccessed. "<<std::endl;
        exit(-1);
    }
    
}


-(void)compute{
    
#if 0
    
    const int ref=_aligner._traverse(path);
    const int nShots=_aligner._getNumShots();
    for(int idx=0;idx<nShots;++idx){
        auto& alignment=_aligner._getAlignment(idx);
        std::string name;
        int nShots=0;
        std::vector<float> add,top,imp,fin;
        ifs.open([csv UTF8String]);
        for(int j=0;j<=ref;++j){
            ifs>>name;
            ifs>>nShots;
            float tmp;
            for(int i=0;i<nShots;++i){
                ifs>>tmp;
                if(j==ref) add.push_back(tmp);
                ifs>>tmp;
                if(j==ref) top.push_back(tmp);
                ifs>>tmp;
                if(j==ref) imp.push_back(tmp);
                ifs>>tmp;
                if(j==ref) fin.push_back(tmp);
            }
        }
        ifs.close();
        int nadd=0,ntop=0,nimp=0,nfin=0;
        float addPos=0,topPos=0,impPos=0,finPos=0;
        for(int t=0;t<alignment.size();++t){
            if(alignment[t]._first==int(add[0]+0.5)){
                addPos+=alignment[t]._second;
                ++nadd;
            }
            if(alignment[t]._first==int(top[0]+0.5)){
                topPos+=alignment[t]._second;
                ++ntop;
            }
            if(alignment[t]._first==int(imp[0]+0.5)){
                impPos+=alignment[t]._second;
                ++nimp;
            }
            if(alignment[t]._first==int(fin[0]+0.5)){
                finPos+=alignment[t]._second;
                ++nfin;
            }
        }
        if(nadd==0) std::cout<<"ERROR: no add"<<std::endl;
        if(ntop==0) std::cout<<"ERROR: no top"<<std::endl;
        if(nimp==0) std::cout<<"ERROR: no imp"<<std::endl;
        if(nfin==0) std::cout<<"ERROR: no fin"<<std::endl;
        addPos/=(float)nadd;
        topPos/=(float)ntop;
        impPos/=(float)nimp;
        finPos/=(float)nfin;
        int start=44100*impPos-128*72;
        if(start<0) start=0;
        int end=44100*impPos+128*72;
        if(end>=_wavLength) end=_wavLength;
        CFFT_Analyzer<float> fft;
        fft._init(128);
        fft._setWindow(FFT_WINDOW_TYPE_HANNING);
        OnsetDetector detector;
        detector._init(128);
        detector._start();
        CVector<float> buf(128);
        CVector<float> sp(2*128);
        std::vector<float> onsets;
        float maxP=0;
        int maxi=-1;
        for(int t=start;t+128<=end;t+=64){
            for(int i=0;i<128;++i) buf[i]=_wav[start+64*t+i];
            fft._process(buf._getPointer(),sp._getPointer());
            detector._process(sp._getPointer());
            if(detector._detected){
                float power=0;
                for(int i=0;i<64;++i) power+=sp[2*i]*sp[2*i]+sp[2*i+1]*sp[2*i+1];
                if(power>maxP){
                    maxP=power;
                    maxi=onsets.size();
                }
                onsets.push_back((start+64*t)/44100.0);
            }
        }
        if(maxi!=-1){
            std::cout<<"Impact Position: "<<onsets[maxi]<<" sec, Power: "<<maxP<<std::endl;
        }else{
            std::cout<<"No impact sound!!!!!!!!!!!!!!!!!!!!"<<std::endl;
        }
    }
    if(nShots>0){
        
    }else{
        std::cout<<"No shots found!!!!!!!!!"<<std::endl;
    }
#endif
    
}


-(void)load{
    
    std::map<std::string,std::vector<int>> shape;
    std::map<std::string,CVector<unsigned char>> data;
    std::string path=_libPath;
    path.append(_target._name);
    path.append(".npz");
    std::cout<<"load "<<path<<std::endl;
    
    loadNPZ(path,shape,data);
    CVector<float> ref;
    move(data["trajectory"],ref);
    const int len=shape["trajectory"][0];
    _skeleton._resize(len);
    for(int i=0;i<len;++i){
        const Skeleton* s=(Skeleton*)&ref[(3*33+3+1)*i];
        _skeleton[i]._visibility=s->_visibility;
        _skeleton[i]._origin=s->_origin;
        for(int j=0;j<33;++j){
            _skeleton[i]._pos[j]=s->_pos[j];
        }
        if(_skeleton[i]._visibility<0){
            for(int k=i-1;k>=0;--k){
                if(_skeleton[k]._visibility>0){
                    for(int j=0;j<33;++j){
                        _skeleton[i]._pos[j]=_skeleton[k]._pos[j];
                    }
                }
            }
        }
    }
    
    move(data["scale"],ref);
    _target._scale=ref[0];
    
}


-(void)loadRef{
    
    std::map<std::string,std::vector<int>> shape;
    std::map<std::string,CVector<unsigned char>> data;
    std::string path=_libPath;
    path.append(_ref._name);
    path.append(".npz");
    std::cout<<"load "<<path<<std::endl;
    
    loadNPZ(path,shape,data);
    CVector<float> ref;
    move(data["trajectory"],ref);
    const int len=shape["trajectory"][0];
    _skeleton_ref._resize(len);
    for(int i=0;i<len;++i){
        const Skeleton* s=(Skeleton*)&ref[(3*33+3+1)*i];
        _skeleton_ref[i]._visibility=s->_visibility;
        _skeleton_ref[i]._origin=s->_origin;
        for(int j=0;j<33;++j){
            _skeleton_ref[i]._pos[j]=s->_pos[j];
        }
        if(_skeleton_ref[i]._visibility<0){
            for(int k=i-1;k>=0;--k){
                if(_skeleton_ref[k]._visibility>0){
                    for(int j=0;j<33;++j){
                        _skeleton_ref[i]._pos[j]=_skeleton_ref[k]._pos[j];
                    }
                }
            }
        }
    }
    
    move(data["scale"],ref);
    _ref._scale=ref[0];
    
}


-(void)begin:(NSString*)filename width:(int)width height:(int)height fps:(float)fps {
    
    _target._name=[filename UTF8String];
    _target._width=width;
    _target._height=height;
    _target._fps=fps;
    _currentFrame=0;
    _resultFrame=0;
    _target._scale=0;
    _trajectory.clear();
    _wavLength=0;
    _wav.clear();
    
}


-(void)end{
    
    if(_currentFrame!=_resultFrame){
        Skeleton s;
        s._visibility=-1;
        _trajectory.push_back(s);
        ++_resultFrame;
    }
    
    std::vector<size_t> shape;
    shape.push_back(_trajectory.size());
    shape.push_back(3*33+3+1);
    std::string path=_libPath;
    path.append(_target._name);
    path.append(".npz");
    std::cout<<"save "<<path<<std::endl;
    saveNPZ<float>(path,"trajectory",(float*)&(_trajectory[0]),shape);
    shape.clear();
    shape.push_back(1);
    saveNPZ<int>(path,"width",&_target._width,shape,true);
    saveNPZ<int>(path,"height",&_target._height,shape,true);
    saveNPZ<float>(path,"fps",&_target._fps,shape,true);
    saveNPZ<float>(path,"scale",&_target._scale,shape,true);
    _trajectory.clear();
    
    NSString* csv=[[NSBundle mainBundle] pathForResource:@"dataset" ofType:@"csv"];
    std::string csvfile=[csv UTF8String];
    std::ifstream ifs(csvfile);
    bool find=false;
    while(!ifs.eof()){
        StrikeData data;
        ifs>>data._name;
        ifs>>data._width;
        ifs>>data._height;
        ifs>>data._fps;
        ifs>>data._nShots;
        float tmp;
        for(int i=0;i<data._nShots;++i){
            ifs>>tmp;
            ifs>>tmp;
            ifs>>tmp;
            ifs>>tmp;
        }
        if(_target._name==data._name){
            find=true;
            break;
        }
    }
    ifs.close();
    
    if(!find){
        std::ofstream ofs(csvfile,std::ios_base::app);
        ofs<<std::endl;
        ofs<<_target._name<<" ";
        ofs<<_target._width<<" "<<_target._height<<" ";
        ofs<<_target._fps<<" ";
        // test
        ofs<<1<<" "<<_trajectory.size()*0.1/_target._fps<<" "<<_trajectory.size()*0.3/_target._fps<<" "<<_trajectory.size()*0.6/_target._fps<<" "<<_trajectory.size()*0.9/_target._fps;
        ofs.close();
    }
    
}


-(void)setAudio:(const int)length wav:(const unsigned short int*)wav{
    
    _wavLength=length;
    _wav.resize(length);
    float maxf=-FLT_MAX;
    float minf=FLT_MAX;
    for(int i=0;i<length;++i){
        _wav[i]=wav[i]/32768.0f;
        if(maxf<_wav[i]) maxf=_wav[i];
        if(minf>_wav[i]) minf=_wav[i];
    }
    for(int i=0;i<length;i++)_wav[i]=2*((_wav[i]-minf)/(maxf-minf)-0.5);
    
}


-(void)addFrame:(CVImageBufferRef)image timestamp:(CMTime)timestamp{
    
    if(_currentFrame!=_resultFrame){
        Skeleton s;
        s._visibility=-1;
        _trajectory.push_back(s);
        std::cout<<"drop"<<std::endl;
        ++_resultFrame;
    }
    
    size_t width  = CVPixelBufferGetWidth(image);
    size_t height = CVPixelBufferGetHeight(image);
    std::string filename=_libPath+std::to_string(_currentFrame)+".npz";
    std::cout<<width<<" "<<height<<" "<<filename<<std::endl;
    
    [_tracker processVideoFrame:(CVImageBufferRef)image timestamp:timestamp];
    
    ++_currentFrame;
    std::cout<<"frameabc: "<<_currentFrame<<std::endl;
}


- (void)poseTracker: (PoseTracker*)poseTracker didOutputLandmarks: (NSArray<Landmark *> *)landmarks{
    
    const int face_index_list[]={0,1,2,3,4,5,6,7,8,9,10};
    const int right_arm_index_list[]={11,13,15,17,19,21};
    const int left_arm_index_list[]={12,14,16,18,20,22};
    const int right_body_side_index_list[]={11,23,25,27,29,31};
    const int left_body_side_index_list[]={12,24,26,28,30,32};
    const int shoulder_index_list[]={11,12};
    const int waist_index_list[]={23,24};
    
    float origin_x=landmarks[27].x;
    float origin_y=landmarks[27].z;
    float origin_z=landmarks[27].y*(-1);
    origin_x+=landmarks[28].x;
    origin_y+=landmarks[28].z;
    origin_z+=landmarks[28].y*(-1);
    origin_x+=landmarks[29].x;
    origin_y+=landmarks[29].z;
    origin_z+=landmarks[29].y*(-1);
    origin_x+=landmarks[30].x;
    origin_y+=landmarks[30].z;
    origin_z+=landmarks[30].y*(-1);
    origin_x+=landmarks[31].x;
    origin_y+=landmarks[31].z;
    origin_z+=landmarks[31].y*(-1);
    origin_x+=landmarks[32].x;
    origin_y+=landmarks[32].z;
    origin_z+=landmarks[32].y*(-1);
    origin_x/=6.0;
    origin_y/=6.0;
    origin_z/=6.0;
    
    Skeleton skeleton;
    skeleton._origin._x=origin_x;
    skeleton._origin._y=origin_y;
    skeleton._origin._z=origin_z;
    for(int i=0;i<33;++i){
        float dx=landmarks[i].x-origin_x;
        float dy=landmarks[i].z-origin_y;
        float dz=landmarks[i].y*(-1)-origin_z;
        float d=sqrt(dx*dx+dy*dy+dz*dz);
        if(d>_target._scale) _target._scale=d;
        skeleton._pos[i]._x=dx;
        skeleton._pos[i]._y=dy;
        skeleton._pos[i]._z=dz;
    }
    _trajectory.push_back(skeleton);
    ++_resultFrame;
    
}


- (void)poseTracker: (PoseTracker*)poseTracker didOutputPixelBuffer: (CVPixelBufferRef)pixelBuffer{}

@end

