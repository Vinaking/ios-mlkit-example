//
//  GolfAlignment-Bridging-Header.h
//  MLKitDemo
//
//  Created by Tùng on 31/10/2022.
//

//#ifndef GolfAlignment_Bridging_Header_h
//#define GolfAlignment_Bridging_Header_h
//
//
//#endif /* GolfAlignment_Bridging_Header_h */



#import <PoseTracker/PoseTracker.h>
#import <CoreVideo/CoreVideo.h>
#import <CoreMedia/CoreMedia.h>
#import <AVFoundation/AVFoundation.h>

@interface GolfAligner : NSObject <TrackerDelegate>

-(void)setLibraryPath:(NSString*)path; // ライブラリのパスを必ず最初に設定

// データセット作成用関数
-(int)getNumDataset;
-(NSString*)getDataset:(int)idx;

-(void)setTargetVideo:(NSString*)filename; // ユーザ動画をセット.
-(int)getNumShots; // ユーザ動画で何回スィングしているか
-(void)setRefVideo:(NSString*)filename; // 比較動画をセット
-(int)getRefNumShots; // 比較動画で何回スィングしているか

-(float)getTopFrame:(int)idx; // ユーザ動画のidx番目のトップのフレーム位置
-(float)getAddFrame:(int)idx; // ユーザ動画のidx番目のアドのフレーム位置
-(float)getImpFrame:(int)idx; // ユーザ動画のidx番目のインパクトのフレーム位置
-(bool)hasImpact:(int)idx; // ユーザ動画のidx番目のショットにインパクト音があるかどうか
-(float)getFinFrame:(int)idx; // ユーザ動画のidx番目のフィニッシュのフレーム位置
-(float)getRefTopFrame:(int)idx; // 比較動画のidx番目のトップのフレーム位置
-(float)getRefAddFrame:(int)idx; // 比較動画のidx番目のアドのフレーム位置
-(float)getRefImpFrame:(int)idx; // 比較動画のidx番目のインパクトのフレーム位置
-(bool)hasRefImpact:(int)idx; // 比較動画のidx番目のショットにインパクト音があるかどうか
-(float)getRefFinFrame:(int)idx; // 比較動画のidx番目のフィニッシュのフレーム位置

// frameフレームでの頭と左右の手の画像内でのピクセル位置. 画面に人が映っていないときはfalseを返す.
-(bool)getSkeltonPos:(int)frame head_x:(int*)head_x head_y:(int*)head_y left_x:(int*)left_x left_y:(int*)left_y right_x:(int*)right_x right_y:(int*)right_y;
-(bool)getRefSkeltonPos:(int)frame head_x:(int*)head_x head_y:(int*)head_y left_x:(int*)left_x left_y:(int*)left_y right_x:(int*)right_x right_y:(int*)right_y;

// 新たな動画の解析. 初めて撮影した動画に対して必ず一度おこなう.
-(void)begin:(NSString*)filename width:(int)width height:(int)height fps:(float)fps; // ファイルオープン直後に呼ぶ
-(void)end; // 全フレームを処理し終わったら呼ぶ
-(void)setAudio:(const int)length wav:(const unsigned short int*)wav; // 音 (44.1kHz, 16bit, モノラル) をセット
-(void)addFrame:(CVImageBufferRef)image timestamp:(CMTime)timestamp; // すべてのフレームに対して処理する
-(void)get4Point;

@end

