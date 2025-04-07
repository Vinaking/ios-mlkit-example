#import <Foundation/Foundation.h>
#import <CoreVideo/CoreVideo.h>
#import <AVFoundation/AVFoundation.h>

@class Landmark;
@class PoseTracker;

@protocol TrackerDelegate <NSObject>
- (void)poseTracker: (PoseTracker*)poseTracker didOutputLandmarks: (NSArray<Landmark*>*)landmarks;
- (void)poseTracker: (PoseTracker*)poseTracker didOutputPixelBuffer: (CVPixelBufferRef)pixelBuffer;
@end

@interface PoseTracker : NSObject
- (instancetype)init;
- (void)startGraph;
- (void)processVideoFrame:(CVPixelBufferRef)imageBuffer timestamp:(CMTime)timestamp;
- (bool)foundLandmark;
- (int)getNumLandmarks;
- (float*)getLandmarks;

@property (weak, nonatomic) id <TrackerDelegate> delegate;
@end

@interface Landmark: NSObject
@property(nonatomic, readonly) float x;
@property(nonatomic, readonly) float y;
@property(nonatomic, readonly) float z;
@end




