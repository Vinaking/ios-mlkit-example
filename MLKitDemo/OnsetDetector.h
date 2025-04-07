//
//  OnsetDetector.h
//  MLKitDemo
//
//  Created by Tùng on 31/10/2022.
//

//#ifndef OnsetDetector_h
//#define OnsetDetector_h
//
//
//#endif /* OnsetDetector_h */


#ifndef OnsetDetector_h
#define OnsetDetector_h


#include "FFT.h"


#define onset_log1 -2.30258509
#define ONSET_LOG_LOWER_LIMIT 2e-42
#define ONSET_LOGOF_LOG_LOWER_LIMIT -96.0154267
#define ONSET_ABSINVOF_LOGOF_LOG_LOWER_LIMIT 0.010414993

class OnsetDetector{

public:

int _sampleRate=44100;
CVector<float> _pool;
float *_mag,*_phase,*_psp,*_odfbuf,*_sortbuf,*_other;
float _floor,_odfparam,_normfactor;
int _windowSize;
int _numbins;
int _medspan;
int _HalfWindowSize;
float _threth;
bool _whiten;
int _mingap;
int _gapleft;
bool _med_odd;
float _logmags;
float _odfvalpost,_odfvalpostprev;
float _dc;
float _nyq;
float _relaxtime;
float _relaxcoef;

bool _detected=false;



void _init(int fftsize,int medspan=11){

int numbins=(fftsize>>1)-1;
int size=(medspan+medspan+fftsize+numbins+2);
_pool._resize(size);

int realnumbins=numbins+2;

_mag=_pool._getPointer();
_phase=_pool._getPointer()+fftsize/2;
_psp=_pool._getPointer()+fftsize;
_odfbuf=_pool._getPointer()+fftsize+realnumbins;
_sortbuf=_pool._getPointer()+fftsize+realnumbins+medspan;
_other=_pool._getPointer()+fftsize+realnumbins+medspan+medspan;
        
_setrelax(1.0f,fftsize>>1);
_floor=0.1f;
_odfparam=0.01;
_normfactor=2560.f/(realnumbins*fftsize);

_odfvalpost=0.f;
_odfvalpostprev=0.f;
_threth=0.2f;
_logmags=false;

_whiten=true;
_detected=false;
_med_odd=(medspan & 1)!=0;
_medspan=medspan;
_mingap=0;
_gapleft=0;

_windowSize=fftsize;
_HalfWindowSize=fftsize/2;
_numbins=numbins;

}


void _setrelax(float time,size_t hopsize){
_relaxtime=time;
_relaxcoef=(time==0.0f)?0.0f:exp((onset_log1*hopsize)/(time*_sampleRate));
}


void _start(){ _pool=0.0f; }


float _process(float* buf){

_detected=false;
_loadframe(buf);
_whitening();
_odf();
_detect();

return(_odfvalpost);
}


void _loadframe(float* buf){

_dc=buf[0];//sqrt(buf[0]*buf[0]+buf[1]*buf[1]);
_nyq=buf[1];//sqrt(buf[2*(_HalfWindowSize-1)]*buf[2*(_HalfWindowSize-1)]+buf[2*(_HalfWindowSize-1)+1]*buf[2*(_HalfWindowSize-1)+1]);

for(int i=0;i<_numbins;i++){
   //_mag[i]=buf[2*i]*buf[2*i]+buf[2*i+1]*buf[2*i+1];//hypotf(buf[2*i+1],buf[2*i]);
   //_mag[i]=log(_mag[i]);//(log(fmax(_mag[i],ONSET_LOG_LOWER_LIMIT))-ONSET_LOGOF_LOG_LOWER_LIMIT)*ONSET_ABSINVOF_LOGOF_LOG_LOWER_LIMIT;
   //_phase[i]=atan2(buf[2*i+1],buf[2*i]);
   _mag[i]=hypotf(buf[2*(i+1)+1],buf[2*(i+1)]);
   //_mag[i]=(log(fmax(_mag[i],ONSET_LOG_LOWER_LIMIT))-ONSET_LOGOF_LOG_LOWER_LIMIT)*ONSET_ABSINVOF_LOGOF_LOG_LOWER_LIMIT;
   _phase[i]=atan2(buf[2*(i+1)+1],buf[2*(i+1)]);
}

}


void _whitening(){

float relaxcoef=_relaxcoef;
float* psp=_psp;
float* pspp1=_psp+1;
float flr=_floor;

float val=fabsf(_dc);
float oldval=psp[0];
if(val<oldval) val+=(oldval-val)*relaxcoef;

psp[0]=val;

val=fabs(_nyq);
oldval=pspp1[_numbins];
if(val<oldval){
  val+=(oldval-val)*relaxcoef;
}
pspp1[_numbins]=val;

for(int i=0;i<_numbins;i++){
   val=fabs(_mag[i]);
   oldval=pspp1[i];
   if(val<oldval){
     val+=(oldval-val)*relaxcoef;
   }
   pspp1[i]=val;
}

_dc/=fmax(flr,psp[0]);
_nyq/=fmax(flr,pspp1[_numbins]);
for(int i=0;i<_numbins;i++){
   _mag[i]/=fmax(flr,pspp1[i]);
}

}


void _odf(){

int numbins=_numbins;
float* val=_odfbuf;
int tbpointer;
float deviation,diff,curmag;
double totdev;
bool rectify=true;
        
memcpy(val+1,val,(_medspan-1)*sizeof(float));

*val=(_nyq*_nyq)+(_dc*_dc);
for(int i=0;i<numbins;i++){
   *val+=_mag[i]*_mag[i];
}

_odfbuf[0]*=_normfactor;

}


void _detect(){

_odfvalpostprev=_odfvalpost;
float* sortbuf=_sortbuf;
int medspan=_medspan;

memcpy(sortbuf,_odfbuf,medspan*sizeof(float));

_SelectionSort(sortbuf,medspan);

if(_med_odd){
  _odfvalpost=_odfbuf[0]-sortbuf[(medspan-1)>>1];
}else{
  _odfvalpost=_odfbuf[0]-((sortbuf[medspan>>1]+sortbuf[(medspan>>1)-1])*0.5f);
}

if(_gapleft!=0){
  _gapleft--;
  _detected=false;
}else{
  _detected=(_odfvalpost>_threth) && (_odfvalpostprev<=_threth);
  if(_detected){
    _gapleft=_mingap;
  }
}

}


void _SelectionSort(float *array,int length){

int max;
float temp;
while(length>0){
     max=0;
     for(int i=1;i<length;i++){
        if(array[i]>array[max]){
          max=i;
        }
     }
     temp=array[length-1];
     array[length-1]=array[max];
     array[max]=temp;
     length--;
}

}
    

};




#endif

