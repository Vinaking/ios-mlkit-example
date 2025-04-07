//
//  CVector.cpp
//  MLKitDemo
//
//  Created by Tùng on 31/10/2022.
//

//#include "CVector.hpp"


#include "CVector.h"

void CreateMetaData(std::string& preamble,std::string& header,const std::string& descriptor,bool fortran_order,std::vector<int>& shape){

header="{'descr': '";
header.append(descriptor);
if(fortran_order) header.append("', 'fortran_order': True, ");
else header.append("', 'fortran_order': False, ");
header.append("'shape': (");

std::stringstream shape_stream;
if(1<shape.size()){
  for(int d=0;d<shape.size();++d){
     shape_stream<<shape[d];
     if(d+1!=shape.size()){
       shape_stream<<",";
     }
  }
}else{
  shape_stream<<shape[0]<<",";
}
shape_stream<<"), }";
header.append(shape_stream.str());
const int to_padding=16-(10+header.size()+1)%16;
for(int m=0;m<to_padding;++m){
   header.push_back(' ');
}
header.push_back('\n');

preamble="\x93NUMPY";
preamble.push_back((char)1);
preamble.push_back((char)0);
uint16_t header_length=header.size();
preamble.append(reinterpret_cast<char*>(&header_length),2);

}


size_t loadNumpy(const std::string& filename,std::vector<int>& shape,CVector<unsigned char>& data){

std::ifstream stream(filename.c_str(),std::ios::in|std::ios::binary);
if(!stream){
  throw std::runtime_error("io error: failed to open a file.");
}

// check if this file is the valid .npy file
std::string valid_preamble="\x93NUMPY";
valid_preamble.push_back(char(1));
valid_preamble.push_back(char(0));
std::string preamble(8,' ');
stream.read(&preamble[0],8);
if(valid_preamble!=preamble){
  throw std::runtime_error(
  "io error: this file do not have a valid npy format.");
}

// load header
uint16_t header_length;
stream.read(reinterpret_cast<char*>(&header_length),sizeof(uint16_t));

if((header_length+preamble.size()+sizeof(uint16_t)) % 16!=0){
  throw std::runtime_error(
  "formatting error: metadata length is not divisible by 16.");
}

std::string header(header_length,' ');
stream.read(reinterpret_cast<char*>(&header[0]),header_length);

// load fortran order
typedef std::string::size_type size_type;
const size_type header_loc=header.find("fortran_order")+16;

// load shape
const size_type shape_loc1=header.find("(");
const size_type shape_loc2=header.find(")");
std::string shape_str=header.substr(shape_loc1+1,shape_loc2-shape_loc1-1);
if(shape_str[shape_str.size()-1]==',') shape.resize(1);
else shape.resize(std::count(shape_str.begin(),shape_str.end(),',')+1);

for(size_t i=0;i<shape.size();++i){
   std::stringstream ss;
   const size_type loc=shape_str.find(",");
   ss<<shape_str.substr(0,loc);
   ss>>shape[i];
   shape_str=shape_str.substr(loc+1);
}

// load descriptor
const size_type descr_loc=header.find("descr")+9;

const char data_type=header[descr_loc+1];
size_t word_size;
std::stringstream ss;
ss<<header[descr_loc+2];
ss>>word_size;

// load data
size_t total=1;
for(size_t i=0;i<shape.size();++i){
   total*=shape[i];
}

data._resize(total*word_size);
stream.read(reinterpret_cast<char*>(&data[0]),word_size*total);

return(word_size);
}


size_t loadNumpy(const std::string& filename,char* data){

std::vector<int> shape;
std::ifstream stream(filename.c_str(),std::ios::in|std::ios::binary);
if(!stream){
  throw std::runtime_error("io error: failed to open a file.");
}

// check if this file is the valid .npy file
std::string valid_preamble="\x93NUMPY";
valid_preamble.push_back(char(1));
valid_preamble.push_back(char(0));
std::string preamble(8,' ');
stream.read(&preamble[0],8);
if(valid_preamble!=preamble){
  throw std::runtime_error(
  "io error: this file do not have a valid npy format.");
}

// load header
uint16_t header_length;
stream.read(reinterpret_cast<char*>(&header_length),sizeof(uint16_t));

if((header_length+preamble.size()+sizeof(uint16_t)) % 16!=0){
  throw std::runtime_error(
  "formatting error: metadata length is not divisible by 16.");
}

std::string header(header_length,' ');
stream.read(reinterpret_cast<char*>(&header[0]),header_length);

// load fortran order
typedef std::string::size_type size_type;
const size_type header_loc=header.find("fortran_order")+16;

// load shape
const size_type shape_loc1=header.find("(");
const size_type shape_loc2=header.find(")");
std::string shape_str=header.substr(shape_loc1+1,shape_loc2-shape_loc1-1);
if(shape_str[shape_str.size()-1]==',') shape.resize(1);
else shape.resize(std::count(shape_str.begin(),shape_str.end(),',')+1);

for(size_t i=0;i<shape.size();++i){
   std::stringstream ss;
   const size_type loc=shape_str.find(",");
   ss<<shape_str.substr(0,loc);
   ss>>shape[i];
   shape_str=shape_str.substr(loc+1);
}

// load descriptor
const size_type descr_loc=header.find("descr")+9;

const char data_type=header[descr_loc+1];
size_t word_size;
std::stringstream ss;
ss<<header[descr_loc+2];
ss>>word_size;

// load data
size_t total=1;
for(size_t i=0;i<shape.size();++i){
   total*=shape[i];
}

stream.read(data,word_size*total);

return(total*word_size);
}


size_t loadNumpy(std::ifstream& stream,std::vector<int>& shape,CVector<unsigned char>& data){

// check if this file is the valid .npy file
std::string valid_preamble="\x93NUMPY";
valid_preamble.push_back(char(1));
valid_preamble.push_back(char(0));
std::string preamble(8,' ');
stream.read(&preamble[0],8);
if(valid_preamble!=preamble){
  throw std::runtime_error(
  "io error: this file do not have a valid npy format.");
}

// load header
uint16_t header_length;
stream.read(reinterpret_cast<char*>(&header_length),sizeof(uint16_t));

if((header_length+preamble.size()+sizeof(uint16_t)) % 16!=0){
  throw std::runtime_error(
  "formatting error: metadata length is not divisible by 16.");
}

std::string header(header_length,' ');
stream.read(reinterpret_cast<char*>(&header[0]),header_length);

// load fortran order
typedef std::string::size_type size_type;
const size_type header_loc=header.find("fortran_order")+16;

// load shape
const size_type shape_loc1=header.find("(");
const size_type shape_loc2=header.find(")");
std::string shape_str=header.substr(shape_loc1+1,shape_loc2-shape_loc1-1);
if(shape_str[shape_str.size()-1]==',') shape.resize(1);
else shape.resize(std::count(shape_str.begin(),shape_str.end(),',')+1);

for(size_t i=0;i<shape.size();++i){
   std::stringstream ss;
   const size_type loc=shape_str.find(",");
   ss<<shape_str.substr(0,loc);
   ss>>shape[i];
   shape_str=shape_str.substr(loc+1);
}

// load descriptor
const size_type descr_loc=header.find("descr")+9;

const char data_type=header[descr_loc+1];
size_t word_size;
std::stringstream ss;
ss<<header[descr_loc+2];
ss>>word_size;

// load data
size_t total=1;
for(size_t i=0;i<shape.size();++i){
   total*=shape[i];
}

data._resize(total*word_size);
stream.read(reinterpret_cast<char*>(&data[0]),word_size*total);

return(word_size);
}


size_t loadNPZ(std::ifstream& stream,const unsigned int compr_bytes,const unsigned int uncompr_bytes,std::vector<int>& shape,CVector<unsigned char>& data){

std::vector<unsigned char> buffer_compr(compr_bytes);
std::vector<unsigned char> buffer_uncompr(uncompr_bytes);
stream.read(reinterpret_cast<char*>(&buffer_compr[0]),compr_bytes);

int err;
z_stream d_stream;
d_stream.zalloc=Z_NULL;
d_stream.zfree=Z_NULL;
d_stream.opaque=Z_NULL;
d_stream.avail_in=0;
d_stream.next_in=Z_NULL;
err=inflateInit2(&d_stream,-MAX_WBITS);
d_stream.avail_in=compr_bytes;
d_stream.next_in=&buffer_compr[0];
d_stream.avail_out=uncompr_bytes;
d_stream.next_out=&buffer_uncompr[0];
err=inflate(&d_stream,Z_FINISH);
err=inflateEnd(&d_stream);

unsigned char* buffer=&buffer_uncompr[0];
// load header
uint16_t header_length=*reinterpret_cast<uint16_t*>(buffer+8);
std::string header(reinterpret_cast<char*>(buffer+9),header_length);
size_t loc1=header.find("fortran_order")+16;
bool fortran_order=(header.substr(loc1,4)=="True"?true:false);

loc1=header.find("(");
size_t loc2=header.find(")");
std::string str_shape=header.substr(loc1+1,loc2-loc1-1);
size_t ndims;
if(str_shape[str_shape.size()-1]==',') ndims=1;
else ndims=std::count(str_shape.begin(),str_shape.end(),',')+1;
shape.resize(ndims);
for(size_t i=0;i<ndims;++i){
   loc1=str_shape.find(",");
   shape[i]=atoi(str_shape.substr(0,loc1).c_str());
   str_shape=str_shape.substr(loc1+1);
}

loc1=header.find("descr")+9;
std::string str_ws = header.substr(loc1+2);
loc2=str_ws.find("'");
size_t word_size=atoi(str_ws.substr(0,loc2).c_str());

// load data
size_t total=1;
for(size_t i=0;i<shape.size();++i){
   total*=shape[i];
}

data._resize(total*word_size);
size_t offset=uncompr_bytes-data._get_size();
memcpy(data._getPointer(),buffer+offset,data._get_size());

return(word_size);
}


void loadNPZ(std::string filename,std::map<std::string,std::vector<int>>& shape,std::map<std::string,CVector<unsigned char>>& data){

std::ifstream stream(filename.c_str(),std::ios::in|std::ios::binary);
if(!stream){
  throw std::runtime_error("io error: failed to open a file.");
}

while(1){
     std::vector<char> local_header(30);
     stream.read(reinterpret_cast<char*>(&local_header[0]),30);
     if(local_header[2]!=0x03 || local_header[3]!=0x04) break;

     uint16_t name_len=*(uint16_t*)&local_header[26];
     std::string varname(name_len,' ');
     stream.read(reinterpret_cast<char*>(&varname[0]),name_len);
     varname.erase(varname.end()-4,varname.end()); // erase the lagging .npy

     uint16_t extra_field_len=*(uint16_t*)&local_header[28];
     if(extra_field_len>0){
       std::vector<char> buf(extra_field_len);
       stream.read(reinterpret_cast<char*>(&buf[0]),extra_field_len);
     }

     unsigned short compr_method=*reinterpret_cast<unsigned short*>(&local_header[0]+8);
     unsigned int compr_bytes=*reinterpret_cast<unsigned int*>(&local_header[0]+18);
     unsigned int uncompr_bytes=*reinterpret_cast<unsigned int*>(&local_header[0]+22);
     
     CVector<unsigned char> v;
     data[varname]=v;
     std::vector<int> shp;
     size_t type;

     if(compr_method==0) type=loadNumpy(stream,shp,data[varname]);
     else type=loadNPZ(stream,compr_bytes,uncompr_bytes,shp,data[varname]);
     shape[varname]=shp;
}

}




void parse_npz_footer(FILE* fp,uint16_t& nrecs,size_t& global_header_size,size_t& global_header_offset){

std::vector<char> footer(22);
fseek(fp,-22,SEEK_END);
size_t res = fread(&footer[0],sizeof(char),22,fp);
if(res!=22) throw std::runtime_error("parse_zip_footer: failed fread");

uint16_t disk_no,disk_start,nrecs_on_disk,comment_len;
disk_no=*(uint16_t*)&footer[4];
disk_start=*(uint16_t*)&footer[6];
nrecs_on_disk=*(uint16_t*)&footer[8];
nrecs=*(uint16_t*)&footer[10];
global_header_size=*(uint32_t*)&footer[12];
global_header_offset=*(uint32_t*)&footer[16];
comment_len=*(uint16_t*)&footer[20];

}


char BigEndianTest(){
int x=1;
return((((char *)&x)[0])?'<':'>');
}

char map_type(const std::type_info& t){

if(t==typeid(float)) return('f');
if(t==typeid(double)) return('f');
if(t==typeid(long double)) return('f');

if(t==typeid(int)) return('i');
if(t==typeid(char)) return('i');
if(t==typeid(short)) return('i');
if(t==typeid(long)) return('i');
if(t==typeid(long long)) return('i');

if(t==typeid(unsigned char)) return('u');
if(t==typeid(unsigned short)) return('u');
if(t==typeid(unsigned long)) return('u');
if(t==typeid(unsigned long long)) return('u');
if(t==typeid(unsigned int)) return('u');

if(t==typeid(bool)) return('b');

if(t==typeid(std::complex<float>)) return('c');
if(t==typeid(std::complex<double>)) return('c');
if(t==typeid(std::complex<long double>)) return('c');
else return '?';
}


std::vector<char>& operator+=(std::vector<char>& lhs,const std::string rhs){
lhs.insert(lhs.end(),rhs.begin(),rhs.end());
return(lhs);
}

std::vector<char>& operator+=(std::vector<char>& lhs,const char* rhs){
size_t len=strlen(rhs);
lhs.reserve(len);
for(size_t byte=0;byte<len;byte++){
   lhs.push_back(rhs[byte]);
}
return(lhs);
}

