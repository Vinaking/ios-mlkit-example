//
//  PlayerViewController.swift
//  MLKitDemo
//
//  Created by Tùng on 28/09/2022.
//

import UIKit
import MediaPlayer
import MobileCoreServices
import AVFoundation
import AVKit
import MLKit
import MLImage


class PlayerViewController: UIViewController {
    
    var avplayer = AVPlayer()
    var playerController = AVPlayerViewController()
    @IBOutlet weak var video_vw: UIView!
    @IBOutlet weak var img: UIImageView!
    
    var videoFPS: Int = 0
    var currentFrame: Int = 0
    var totalFrames: Int?
    var asset: AVURLAsset!
    var generator:AVAssetImageGenerator!
    
    var isPlayvideo:Bool = false
    
    private var poseDetector: PoseDetector? = nil
    var resultsText = ""
    
    private lazy var annotationOverlayView: UIView = {
        precondition(isViewLoaded)
        let annotationOverlayView = UIView(frame: .zero)
        annotationOverlayView.translatesAutoresizingMaskIntoConstraints = false
        annotationOverlayView.clipsToBounds = true
        return annotationOverlayView
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        
        
        let options = AccuratePoseDetectorOptions()
        options.detectorMode = .singleImage
        poseDetector = PoseDetector.poseDetector(options: options)
        //        detectPose(image: img.image)
    }
    
    override func viewDidAppear(_ animated: Bool) {
        
    }
    
    @IBAction func galleryAction(_ sender: Any) {
        let videoPickerController = UIImagePickerController()
        videoPickerController.delegate = self
        videoPickerController.transitioningDelegate = self
        if UIImagePickerController.isSourceTypeAvailable(.photoLibrary) == false { return }
        videoPickerController.allowsEditing = true
        videoPickerController.sourceType = .photoLibrary
        videoPickerController.videoMaximumDuration = TimeInterval(240.0)
        videoPickerController.mediaTypes = [kUTTypeMovie as String]
        videoPickerController.modalPresentationStyle = .custom
        self.present(videoPickerController, animated: true, completion: nil)
    }
    
    func addVideoPlayer(videoUrl: URL, to view: UIView) {
        //        self.avplayer = AVPlayer(url: videoUrl)
        //        playerController.player = self.avplayer
        configureVideo(videoURL: videoUrl)
        self.addChild(playerController)
        view.addSubview(playerController.view)
        playerController.view.frame = view.bounds
        playerController.showsPlaybackControls = true
        self.avplayer.play()
        
        video_vw.addSubview(annotationOverlayView)
        NSLayoutConstraint.activate([
            annotationOverlayView.topAnchor.constraint(equalTo: video_vw.topAnchor),
            annotationOverlayView.leadingAnchor.constraint(equalTo: video_vw.leadingAnchor),
            annotationOverlayView.trailingAnchor.constraint(equalTo: video_vw.trailingAnchor),
            annotationOverlayView.bottomAnchor.constraint(equalTo: video_vw.bottomAnchor),
        ])
        
    }
    
}

extension PlayerViewController: UIImagePickerControllerDelegate, UINavigationControllerDelegate, UIViewControllerTransitioningDelegate {
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [UIImagePickerController.InfoKey : Any]) {
        let videoURL = info[UIImagePickerController.InfoKey.mediaURL] as? URL
        print(videoURL?.path ?? "")
        self.dismiss(animated: true, completion: nil)
        
        if let videourl = videoURL {
            self.addVideoPlayer(videoUrl: videourl, to: video_vw)
        }
    }
}

extension PlayerViewController {
    private func loadAsset(url: URL) {
        let asset = AVAsset(url: url)
        let reader = try! AVAssetReader(asset: asset)
        
        let videoTrack = asset.tracks(withMediaType: AVMediaType.video)[0]
        
        // read video frames as BGRA
        let trackReaderOutput = AVAssetReaderTrackOutput(track: videoTrack, outputSettings:[String(kCVPixelBufferPixelFormatTypeKey): NSNumber(value: kCVPixelFormatType_32BGRA)])
        
        reader.add(trackReaderOutput)
        reader.startReading()
        
        while let sampleBuffer = trackReaderOutput.copyNextSampleBuffer() {
            print("sample at time \(CMSampleBufferGetPresentationTimeStamp(sampleBuffer))")
            if let imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer) {
                // process each CVPixelBufferRef here
                // see CVPixelBufferGetWidth, CVPixelBufferLockBaseAddress, CVPixelBufferGetBaseAddress, etc
            }
        }
    }
    
    
    func configureVideo(videoURL: URL) {
        asset = AVURLAsset(url: videoURL)
        generator = AVAssetImageGenerator(asset: asset)
        generator.appliesPreferredTrackTransform = true
        avplayer = AVPlayer(url: videoURL)
        playerController.player = avplayer
        guard avplayer.currentItem?.asset != nil else {
            return
        }
        let asset = self.avplayer.currentItem?.asset
        let tracks = asset!.tracks(withMediaType: .video)
        let fps = tracks.first?.nominalFrameRate
        
        self.videoFPS = lround(Double(fps!))
        self.getVideoData()
        print("fps: \(videoFPS)")
        
    }
    
    func getVideoData() {
        //        do {
        //            let timestamp = CMTime(seconds: 2, preferredTimescale: 60)
        //            let imageRef = try self.generator.copyCGImage(at: timestamp, actualTime: nil)
        //            let image = UIImage(cgImage: imageRef)
        //
        //            img.image = image
        //            detectPose(image: image)
        //
        //        }
        //        catch let error as NSError
        //        {
        //            print("Image generation failed with error \(error)")
        //        }
        
//                let interval = CMTime(seconds: 1, preferredTimescale: 1)
        
        //        let interval = CMTimeMake(value: 1 ,timescale: Int32(self.videoFPS))
        
        let interval = CMTimeMake(value: 1 ,timescale: 600000)
        var dem: Int = 0
        
        //        let interval = CMTimeMakeWithSeconds(0.05 ,preferredTimescale: Int32(self.videoFPS))
        
        
        self.avplayer.addPeriodicTimeObserver(forInterval: interval, queue: DispatchQueue.main) {[weak self] (progressTime) in
            dem+=1
            print("time: \(dem)")
            self!.genetorImage()
            
        }
        
        //        var times = [NSValue]()
        //
        //        var currentTime = CMTime.zero // make your time here
        //
        //                times.append(NSValue(time:currentTime))
        //
        //        self.avplayer.addBoundaryTimeObserver(forTimes: times, queue: DispatchQueue.main) {
        //            DispatchQueue.main.async {
        //                self.genetorImage(frame: 1)
        //            }
        //        }
    }
    
    func genetorImage() {
        do {
            var actualTime: CMTime = .indefinite
            let imageRef = try self.generator.copyCGImage(at: self.avplayer.currentTime(), actualTime: nil)
            let image = UIImage(cgImage: imageRef)
            detectPose(image: image)
        }
        catch let error as NSError
        {
            print("Image generation failed with error \(error)")
        }
    }
    
    func detectPose(image: UIImage?) {
        removeDetectionAnnotations()
        guard let image = updateImageView(with: image) else { return }
        
        guard let inputImage = MLImage(image: image) else {
            print("Failed to create MLImage from UIImage.")
            return
        }
        inputImage.orientation = image.imageOrientation
        
        if let poseDetector = self.poseDetector {
            poseDetector.process(inputImage) { poses, error in
                guard error == nil, let poses = poses, !poses.isEmpty else {
                    let errorString = error?.localizedDescription ?? Constants.detectionNoResultsMessage
                    self.resultsText = "Pose detection failed with error: \(errorString)"
                    //                    self.showResults()
                    return
                }
                let transform = self.transformMatrix(image: image)
                
                // Pose detected. Currently, only single person detection is supported.
                poses.forEach { pose in
                    let poseOverlayView = UIUtilities.createPoseOverlayView(
                        forPose: pose,
                        inViewWithBounds: self.annotationOverlayView.bounds,
                        lineWidth: Constants.lineWidth,
                        dotRadius: Constants.smallDotRadius,
                        positionTransformationClosure: { (position) -> CGPoint in
                            return self.pointFrom(position).applying(transform)
                        }
                    )
                    self.annotationOverlayView.addSubview(poseOverlayView)
                    self.resultsText = "Pose Detected"
                    //                    self.showResults()
                }
            }
        }
    }
    
    private func updateImageView(with image: UIImage?) -> UIImage? {
        let orientation = UIApplication.shared.statusBarOrientation
        var scaledImageWidth: CGFloat = 0.0
        var scaledImageHeight: CGFloat = 0.0
        switch orientation {
        case .portrait, .portraitUpsideDown, .unknown:
            scaledImageWidth = video_vw.bounds.size.width
            scaledImageHeight = (image?.size.height ?? 0) * scaledImageWidth / (image?.size.width ?? 0)
        case .landscapeLeft, .landscapeRight:
            scaledImageWidth = (image?.size.width ?? 0) * scaledImageHeight / (image?.size.height ?? 0)
            scaledImageHeight = video_vw.bounds.size.height
        @unknown default:
            fatalError()
        }
        weak var weakSelf = self
        DispatchQueue.global(qos: .userInitiated).async {
            // Scale image while maintaining aspect ratio so it displays better in the UIImageView.
            
        }
        
        let scaledImage = image?.scaledImage(
            with: CGSize(width: scaledImageWidth, height: scaledImageHeight)
        )
        return scaledImage ?? image
    }
    
    private func pointFrom(_ visionPoint: VisionPoint) -> CGPoint {
        return CGPoint(x: visionPoint.x, y: visionPoint.y)
    }
    
    private func showResults() {
        let resultsAlertController = UIAlertController(
            title: "Detection Results",
            message: nil,
            preferredStyle: .actionSheet
        )
        resultsAlertController.addAction(
            UIAlertAction(title: "OK", style: .destructive) { _ in
                resultsAlertController.dismiss(animated: true, completion: nil)
            }
        )
        resultsAlertController.message = resultsText
        //      resultsAlertController.popoverPresentationController?.barButtonItem = detectButton
        resultsAlertController.popoverPresentationController?.sourceView = self.view
        present(resultsAlertController, animated: true, completion: nil)
        print(resultsText)
    }
    
    private func transformMatrix(image: UIImage) -> CGAffineTransform {
        let imageViewWidth = video_vw.frame.size.width
        let imageViewHeight = video_vw.frame.size.height
        let imageWidth = image.size.width
        let imageHeight = image.size.height
        
        let imageViewAspectRatio = imageViewWidth / imageViewHeight
        let imageAspectRatio = imageWidth / imageHeight
        let scale =
        (imageViewAspectRatio > imageAspectRatio)
        ? imageViewHeight / imageHeight : imageViewWidth / imageWidth
        
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
    
    private func removeDetectionAnnotations() {
        for annotationView in annotationOverlayView.subviews {
            annotationView.removeFromSuperview()
        }
    }
}

private enum Constants {
    static let images = [
        "grace_hopper.jpg", "image_has_text.jpg", "chinese_sparse.png", "chinese.png",
        "devanagari_sparse.png", "devanagari.png", "japanese_sparse.png", "japanese.png",
        "korean_sparse.png", "korean.png", "barcode_128.png", "qr_code.jpg", "beach.jpg", "liberty.jpg",
        "bird.jpg",
    ]
    
    static let detectionNoResultsMessage = "No results returned."
    static let failedToDetectObjectsMessage = "Failed to detect objects in image."
    static let localModelFile = (name: "bird", type: "tflite")
    static let labelConfidenceThreshold = 0.75
    static let smallDotRadius: CGFloat = 5.0
    static let largeDotRadius: CGFloat = 10.0
    static let lineColor = UIColor.yellow.cgColor
    static let lineWidth: CGFloat = 3.0
    static let fillColor = UIColor.clear.cgColor
    static let segmentationMaskAlpha: CGFloat = 0.5
}
