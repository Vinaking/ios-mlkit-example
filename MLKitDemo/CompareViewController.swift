//
//  CompareViewController.swift
//  MLKitDemo
//
//  Created by Tùng on 24/10/2022.
//

import UIKit
import MediaPlayer
import MobileCoreServices
import AVFoundation
import AVKit
import MLKit
import MLImage
import AssetsLibrary
import mobileffmpeg
import PhotosUI

class CompareViewController: UIViewController, UIScrollViewDelegate {
    @IBOutlet private var wrapVideoView: UIStackView!
    @IBOutlet private var wrapVideoView1: UIView!
    @IBOutlet private var videoView1: UIView!
    @IBOutlet private var videoView2: UIView!
    @IBOutlet private var sclView: UIScrollView!
    @IBOutlet private var sclView2: UIScrollView!
    
    @IBOutlet private var seekSlider1: UISlider!
    @IBOutlet private var seekSlider2: UISlider!
    
    private var player1: AVPlayer!
    private var player2: AVPlayer!
    
    private lazy var sessionQueue = DispatchQueue(label: "com.google.mlkit.visiondetector.SessionQueue")
    
    private var videoUrl1: URL? = nil
    private var videoUrl2: URL? = nil
    let playerLayer1 = AVPlayerLayer()
    let playerLayer2 = AVPlayerLayer()
    
    var pointsHand = [CGPoint]()
    
    private var poseDetector: PoseDetector? = nil
    
    var xFooter: Double = 0
    var yFooter: Double = 0
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let options = AccuratePoseDetectorOptions()
        poseDetector = PoseDetector.poseDetector(options: options)
        
        sclView.delegate = self

        sclView.minimumZoomScale = 0.1
        sclView.maximumZoomScale = 10.0 //maximum zoom scale you want
        sclView.zoomScale = 2
        sclView.showsHorizontalScrollIndicator = false
        sclView.showsVerticalScrollIndicator = false
//        sclView.panGestureRecognizer.minimumNumberOfTouches = 2
        
//        let offsetX = max((sclView.bounds.width - sclView.contentSize.width) * 0.3, 0)
//        let offsetY = max((sclView.bounds.height - sclView.contentSize.height) * 0.5, 0)
//        sclView.contentInset = UIEdgeInsets(top: offsetY, left: offsetX, bottom: 0, right: 0)
        
        sclView2.delegate = self

        sclView2.minimumZoomScale = 0.1
        sclView2.maximumZoomScale = 10.0 //maximum zoom scale you want
        sclView2.zoomScale = 1.0
        sclView2.showsHorizontalScrollIndicator = false
        sclView2.showsVerticalScrollIndicator = false
        
        
        
    }
    
    override func viewDidLayoutSubviews() {
        let widthVideoFrame = wrapVideoView1.frame.width
        let heightVideoFrame = wrapVideoView1.bounds.height
        
        xFooter = widthVideoFrame*25/100
        yFooter = heightVideoFrame*85/100
        
//        xFooter = 0
//        yFooter = 0
        
        let pointFooter = CGPoint(x: xFooter, y: yFooter)
        createPoint(point: pointFooter)
        
        print("sizelog: xFooter: \(xFooter)")
        print("sizelog: yFooter: \(yFooter)")
        
//        sclView.contentOffset = pointFooter
    }
    
    func viewForZooming(in scrollView: UIScrollView) -> UIView? {
        if scrollView == sclView {
            return videoView1
        }else {
            return videoView2
        }
    }
    
    func scrollViewDidScroll(_ scrollView: UIScrollView) {
        
    }
    
    func scrollViewDidZoom(_ scrollView: UIScrollView) {
        let contentSize = scrollView.contentSize.width
        let videoSize = videoView1.frame.width
        
        let offsetX = max((scrollView.bounds.width - scrollView.contentSize.width) * 0.5, 0)
        let offsetY = max((scrollView.bounds.height - scrollView.contentSize.height) * 0.5, 0)
        print("sizelog: content: \(contentSize)")
        print("sizelog: contentOffset: \(scrollView.contentOffset.x), \(scrollView.contentOffset.y)")
//        print("sizelog: videosize: \(videoSize)")
//        print("sizelog: offsetX: \(offsetX)")
//        print("sizelog: offsetY: \(offsetY)")
        
        
        scrollView.contentInset = UIEdgeInsets(top: offsetY, left: offsetX, bottom: 0, right: 0)
    }
    
    override func viewWillTransition(to size: CGSize, with coordinator: UIViewControllerTransitionCoordinator) {
        super.viewWillTransition(to: size, with: coordinator)
        if UIDevice.current.orientation.isLandscape {
            print("Landscape")
            wrapVideoView.axis = .horizontal
        } else {
            print("Portrait")
            wrapVideoView.axis = .vertical
        }
    }
    
    @IBAction func galleryAction(_ sender: Any) {
        let videoPickerController = UIImagePickerController()
        videoPickerController.sourceType = .savedPhotosAlbum
        videoPickerController.delegate = self
        videoPickerController.mediaTypes = [kUTTypeMovie as String]
        
        self.present(videoPickerController, animated: true, completion: nil)
    }
    
    @IBAction func addressClick(_ sender: Any) {
        player1.seek(to: CMTimeMake(value: 2, timescale: 1))
        self.player1.pause()
        
        player2.seek(to: CMTimeMake(value: 1, timescale: 1))
        self.player2.pause()
    }
    
    @IBAction func topClick(_ sender: Any) {
        let time1 = CMTimeMake(value: 2690, timescale: 1000)
        player1.seek(to: time1, toleranceBefore: CMTime.zero, toleranceAfter: CMTime.zero)
        self.player1.pause()
        
        let time2 = CMTimeMake(value: 1950, timescale: 1000)
        player2.seek(to: time2, toleranceBefore: CMTime.zero, toleranceAfter: CMTime.zero)
        self.player2.pause()
    }
    
    @IBAction func impactClick(_ sender: Any) {
        let time1 = CMTimeMake(value: 2920, timescale: 1000)
        player1.seek(to: time1, toleranceBefore: CMTime.zero, toleranceAfter: CMTime.zero)
        self.player1.pause()
        
        let time2 = CMTimeMake(value: 2290, timescale: 1000)
        player2.seek(to: time2, toleranceBefore: CMTime.zero, toleranceAfter: CMTime.zero)
        self.player2.pause()
    }
    
    @IBAction func finishClick(_ sender: Any) {
        player1.seek(to: CMTimeMake(value: 5, timescale: 1))
        self.player1.pause()
        
        player2.seek(to: CMTimeMake(value: 4, timescale: 1))
        self.player2.pause()
    }
    
    @IBAction func stopPlayClick(_ sender: Any) {
        player1.play()
        player2.play()
    }
    
    func playVideo1(videoURL: URL) {
//        sclView.layer.addSublayer(playerLayer1)
        videoView1.layer.addSublayer(playerLayer1)
        playerLayer1.frame = videoView1.bounds
        
        let asset = AVAsset(url: videoURL)
        let item = AVPlayerItem(asset: asset)
        player1 = AVPlayer(playerItem: item)
        print("timeVideo: \(item.duration.seconds)")
        
        // 3
        player1.actionAtItemEnd = .none
        
        // 4
        player1.volume = 1.0
        player1.rate = 1.0
        
        playerLayer1.player = player1
        player1.pause()
        addTimeObserver1()
        
        let mediainfo = MobileFFprobe.getMediaInformation(videoURL.path)
        print("metadata: \(mediainfo?.getAllProperties())")
        
        let tracks = asset.tracks(withMediaType: .video)
        let fps = tracks.first?.nominalFrameRate
        let a = lround(Double(fps!))
        print("fps: \(String(describing: a))")
        print("fps: \(asset.duration)")
        
        
    }
    
    func playVideo2(videoURL: URL) {
        
        videoView2.layer.addSublayer(playerLayer2)
        playerLayer2.frame = videoView2.bounds
        
        let asset = AVAsset(url: videoURL)
        let item = AVPlayerItem(asset: asset)
        
        player2 = AVPlayer(playerItem: item)
        print("timeVideo: \(item.duration.seconds)")
        print("fps: \(asset.duration)")
        
        // 3
        player2.actionAtItemEnd = .none
        
        // 4
        player2.volume = 1.0
        player2.rate = 1.0
        
        playerLayer2.player = player2
        player2.pause()
        addTimeObserver2()
    }
    
    func addTimeObserver1() {
        let interval = CMTime(seconds: 0.01, preferredTimescale: CMTimeScale(NSEC_PER_SEC))
        let mainQueue = DispatchQueue.main
        player1.addPeriodicTimeObserver(forInterval: interval, queue: mainQueue) { [weak self] time in
            guard let currentItem = self!.player1.currentItem else {return}
            if self?.player1.currentItem!.status == .readyToPlay {
                self?.seekSlider1.minimumValue = 0
                self?.seekSlider1.maximumValue = Float(currentItem.duration.seconds)
                self?.seekSlider1.value = Float(currentItem.currentTime().seconds)
            }
        }
    }
    
    func addTimeObserver2() {
        let interval = CMTime(seconds: 0.01, preferredTimescale: CMTimeScale(NSEC_PER_SEC))
        let mainQueue = DispatchQueue.main
        player2.addPeriodicTimeObserver(forInterval: interval, queue: mainQueue) { [weak self] time in
            guard let currentItem = self!.player2.currentItem else {return}
            if self?.player2.currentItem!.status == .readyToPlay {
                self?.seekSlider2.minimumValue = 0
                self?.seekSlider2.maximumValue = Float(currentItem.duration.seconds)
                self?.seekSlider2.value = Float(currentItem.currentTime().seconds)
            }
        }
    }
}

// MARK: Picker
extension CompareViewController: UIImagePickerControllerDelegate, UINavigationControllerDelegate, UIViewControllerTransitioningDelegate {
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [UIImagePickerController.InfoKey : Any]) {
        self.dismiss(animated: true, completion: nil)
        
        let videoURL = info[UIImagePickerController.InfoKey.mediaURL] as? URL
        if let videourl = videoURL {
            playVideo(url: videourl)
        }
        
//        let phAsset = info[UIImagePickerController.InfoKey.phAsset] as? PHAsset
//        if let asset = phAsset {
//            VideoOriginal.share.getVideoOriginal(asset: asset)
//            VideoOriginal.share.originalURL = { url in
//                DispatchQueue.main.async {
//                    self.playVideo(url: url)
//                }
//            }
//        }
    }
    
    private func playVideo(url: URL) {
        if videoUrl1 == nil {
            videoUrl1 = url
            playVideo1(videoURL: url)
//            loadAsset(url: videoUrl1!)
            
        } else {
            videoUrl2 = url
            playVideo2(videoURL: url)
        }
    }
}

// MARK: MLKit
extension CompareViewController {
    private func detectMovement(in image: MLImage, width: CGFloat, height: CGFloat) {
        
        if let poseDetector = self.poseDetector {
            var poses: [Pose] = []
            var detectionError: Error?
            do {
                poses = try poseDetector.results(in: image)
            } catch let error {
                detectionError = error
            }
            weak var weakSelf = self
            DispatchQueue.main.sync {
                guard let strongSelf = weakSelf else {
                    print("Self is nil!")
                    return
                }
                if let detectionError = detectionError {
                    print("Failed to detect poses with error: \(detectionError.localizedDescription).")
                    return
                }
                guard !poses.isEmpty else {
                    print("Pose detector returned no results.")
                    return
                }
                
                let transform = self.transformMatrix(width: width, height: height)
                
                poses.forEach { pose in
                    
                    let handOverlayView = UIUtilities.createMoveHandOverlayView(
                        fromPoint: strongSelf.pointsHand.last,
                        forPose: pose,
                        inViewWithBounds: strongSelf.videoView1.bounds,
                        lineWidth: Constants.lineWidthBold,
                        dotRadius: Constants.smallDotRadius,
                        positionTransformationClosure: { (position) -> CGPoint in
                            let point = strongSelf.pointFrom(position).applying(transform)
                            self.pointsHand.append(point)
                            return point
                        })
                    //                    strongSelf.handOverlayView.addSubview(handOverlayView)
                }
            }
            
        }
    }
    
    private func loadAsset(url: URL) {
        sessionQueue.async {
            let asset = AVAsset(url: url)
            
            let reader = try! AVAssetReader(asset: asset)
            
            //        reader.timeRange.duration = CMTimeMake(value: 0, timescale: 30)
            
            let videoTrack = asset.tracks(withMediaType: AVMediaType.video)[0]
            
            // read video frames as BGRA
            let trackReaderOutput = AVAssetReaderTrackOutput(track: videoTrack, outputSettings:[String(kCVPixelBufferPixelFormatTypeKey): NSNumber(value: kCVPixelFormatType_32BGRA)])
            
            reader.add(trackReaderOutput)
            reader.startReading()
            while let sampleBuffer = trackReaderOutput.copyNextSampleBuffer() {
                
                if let imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer) {
                    let time = CMSampleBufferGetPresentationTimeStamp(sampleBuffer)
                    print("timemeta: \(time)")
                    let imageWidth = CGFloat(CVPixelBufferGetWidth(imageBuffer))
                    let imageHeight = CGFloat(CVPixelBufferGetHeight(imageBuffer))
                    
                    guard let  inputImage = MLImage(sampleBuffer: sampleBuffer) else {
                        print("Failed to create MLImage from UIImage.")
                        return
                    }
                    self.detectMovement(in: inputImage, width: imageWidth, height: imageHeight)
                    
                }
            }
            
            if reader.status == AVAssetReader.Status.completed {
                DispatchQueue.main.async {
                    self.drawHand(data: self.pointsHand, color: UIColor.red.cgColor)
                }
            }
            
        }
    }
    
    private func pointFrom(_ visionPoint: VisionPoint) -> CGPoint {
        return CGPoint(x: visionPoint.x, y: visionPoint.y)
    }
    
    func loadSlowMotion(url: URL) {
        sessionQueue.async {
            let asset = AVAsset(url: url)
            let videoComposition = AVMutableVideoComposition(propertiesOf: asset)
            print("frame: \(videoComposition.frameDuration)")
                                    
            let exportSession = AVAssetExportSession(asset: asset, presetName: AVAssetExportPresetHighestQuality)

            exportSession?.videoComposition = videoComposition

            exportSession?.outputURL = self.temporaryURL()
            exportSession?.outputFileType = AVFileType.mov
            exportSession?.exportAsynchronously(completionHandler: {
                if let url = exportSession?.outputURL {
                    DispatchQueue.main.async {
                        self.loadAsset(url: url)
                        
                    }
                }
                
            })
        }
        
    }
    
    func temporaryURL() -> URL? {
        let directory = try! FileManager.default.url(for:.documentDirectory, in: .userDomainMask, appropriateFor: nil, create: true)
        
        let videoURL = directory.appendingPathComponent(NSUUID().uuidString + ".mp4")
        return videoURL
    }
    
    private func transformMatrix(width: CGFloat, height: CGFloat) -> CGAffineTransform {
        let imageViewWidth = videoView1.frame.size.width
        let imageViewHeight = videoView1.frame.size.height
        
        let imageWidth = width
        let imageHeight = height
        
        let imageViewAspectRatio = imageViewWidth / imageViewHeight
        let imageAspectRatio = imageWidth / imageHeight
        let scale = (imageViewAspectRatio > imageAspectRatio) ? imageViewHeight / imageHeight : imageViewWidth / imageWidth
        
        // Image view's `contentMode` is `scaleAspectFit`, which scales the image to fit the size of the
        // image view by maintaining the aspect ratio. Multiple by `scale` to get image's original size.
        let scaledImageWidth = imageWidth * scale
        let scaledImageHeight = imageHeight * scale
        let xValue = (imageViewWidth - scaledImageWidth) / CGFloat(2.0)
        let yValue = (imageViewHeight - scaledImageHeight) / CGFloat(2.0)
        
        var transform = CGAffineTransform.identity.translatedBy(x: xValue, y: yValue)
        transform = transform.scaledBy(x: scale, y: scale)
        return transform
    }
    
    func drawHand(data: [CGPoint], color: CGColor) {
        let path = UIBezierPath()
        let config = BezierConfiguration()
        let controlPoints = config.configureControlPoints(data: data)
        for i in 0..<data.count {
            let point = data[i]
            //            print("pointa[\(i)]: \(point)")
            if i == 0 {
                path.move(to: point)
            }else {
                let segment = controlPoints[i - 1]
                path.addCurve(to: point, controlPoint1: segment.firstControlPoint, controlPoint2: segment.secondControlPoint)
            }
        }
        
        let _shapeLayer = CAShapeLayer()
        
        _shapeLayer.strokeColor = color
        _shapeLayer.fillColor = nil
        _shapeLayer.lineWidth = 3
        _shapeLayer.path = path.cgPath
        _shapeLayer.lineCap = CAShapeLayerLineCap.round
        videoView1.layer.addSublayer(_shapeLayer)
    }
    
    func createPoint(point: CGPoint) {
        
        let circleLayerShadow = CAShapeLayer()
        circleLayerShadow.bounds = CGRect(x: 0, y: 0, width: 20, height: 20)
        circleLayerShadow.path = UIBezierPath(ovalIn: circleLayerShadow.bounds).cgPath
        circleLayerShadow.fillColor = UIColor.green.cgColor
        circleLayerShadow.position = point
        circleLayerShadow.name = "circle"
        circleLayerShadow.opacity = 0.5
        wrapVideoView1.layer.addSublayer(circleLayerShadow)
        
        let circleLayer = CAShapeLayer()
        circleLayer.bounds = CGRect(x: 0, y: 0, width: 10, height: 10)
        circleLayer.path = UIBezierPath(ovalIn: circleLayer.bounds).cgPath
        circleLayer.fillColor = UIColor.green.cgColor
        circleLayer.position = point
        circleLayer.name = "circle"
        wrapVideoView1.layer.addSublayer(circleLayer)
    }
}

private enum Constants {
    
    static let detectionNoResultsMessage = "No results returned."
    static let failedToDetectObjectsMessage = "Failed to detect objects in image."
    static let localModelFile = (name: "bird", type: "tflite")
    static let labelConfidenceThreshold = 0.75
    static let smallDotRadius: CGFloat = 5.0
    static let largeDotRadius: CGFloat = 10.0
    static let lineColor = UIColor.yellow.cgColor
    static let lineWidth: CGFloat = 3.0
    static let lineWidthBold: CGFloat = 3.0
    static let fillColor = UIColor.clear.cgColor
    static let segmentationMaskAlpha: CGFloat = 0.5
}
