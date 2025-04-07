//
//  Asset+Extension.swift
//  MLKitDemo
//
//  Created by Tùng on 13/10/2022.
//

import Foundation
import AVKit
import Photos

extension AVAsset {
    
    func videoOrientation() -> (orientation: UIInterfaceOrientation, device: AVCaptureDevice.Position) {
        var orientation: UIInterfaceOrientation = .unknown
        var device: AVCaptureDevice.Position = .unspecified
        
        let tracks :[AVAssetTrack] = self.tracks(withMediaType: AVMediaType.video)
        if let videoTrack = tracks.first {
            
            let t = videoTrack.preferredTransform
            
            if (t.a == 0 && t.b == 1.0 && t.d == 0) {
                orientation = .portrait
                
                if t.c == 1.0 {
                    device = .front
                } else if t.c == -1.0 {
                    device = .back
                }
            }
            else if (t.a == 0 && t.b == -1.0 && t.d == 0) {
                orientation = .portraitUpsideDown
                
                if t.c == -1.0 {
                    device = .front
                } else if t.c == 1.0 {
                    device = .back
                }
            }
            else if (t.a == 1.0 && t.b == 0 && t.c == 0) {
                orientation = .landscapeRight
                
                if t.d == -1.0 {
                    device = .front
                } else if t.d == 1.0 {
                    device = .back
                }
            }
            else if (t.a == -1.0 && t.b == 0 && t.c == 0) {
                orientation = .landscapeLeft
                
                if t.d == 1.0 {
                    device = .front
                } else if t.d == -1.0 {
                    device = .back
                }
            }
        }
        
        return (orientation, device)
    }
}

class VideoOriginal: NSObject {
    static let share = VideoOriginal()
    var originalURL: ((URL) -> Void)?
    
    func getVideoOriginal(asset: PHAsset) {
        let videoOptions = PHVideoRequestOptions()
        videoOptions.version = PHVideoRequestOptionsVersion.original
        PHImageManager.default().requestAVAsset(forVideo: asset, options: videoOptions , resultHandler: { (asset, audioMix, info) -> Void in
            if let asset = asset as? AVURLAsset {
                self.exportVideo(asset: asset)
//                let videoData = NSData(contentsOf: asset.url)
//                let videoPath = NSTemporaryDirectory() + "tmpMovie.MOV"
//                let vURL = NSURL(fileURLWithPath: videoPath)
//                let writeResult = videoData?.write(to: vURL as URL, atomically: true)
//
//                if let writeResult = writeResult, writeResult {
//                    print("success")
//
//                    let videoURL = vURL as URL
//                    self.exportVideo(url: videoURL)
//                }
//                else {
//                    print("failure")
//                }
            }
        })
    }
    
    func exportVideo(asset: AVURLAsset) {
        let videoComposition = AVMutableVideoComposition(propertiesOf: asset)
        print("frame: \(videoComposition.frameDuration)")
        
        let exportSession = AVAssetExportSession(asset: asset, presetName: AVAssetExportPresetHighestQuality)
        
        exportSession?.videoComposition = videoComposition
        
        exportSession?.outputURL = self.temporaryURL()
        exportSession?.outputFileType = AVFileType.mov
        exportSession?.exportAsynchronously(completionHandler: {
            if let url = exportSession?.outputURL {
                self.originalURL?(url)
            }
            
        })
        
    }
    
    func temporaryURL() -> URL? {
        let directory = try! FileManager.default.url(for:.documentDirectory, in: .userDomainMask, appropriateFor: nil, create: true)
        let videoURL = directory.appendingPathComponent(NSUUID().uuidString + ".mov")
        return videoURL
    }
    
}
