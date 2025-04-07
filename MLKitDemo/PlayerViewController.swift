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
    
    private lazy var sessionQueue = DispatchQueue(label: "com.google.mlkit.visiondetector.SessionQueue")
    private lazy var videoCompressQueue = DispatchQueue(label: "videoCompressQueue", attributes: .concurrent)
    
    var videoFPS: Int = 0
    var currentFrame: Int = 0
    var totalFrames: Int?
    var asset: AVURLAsset!
    var generator:AVAssetImageGenerator!
    
    var isPlayvideo:Bool = false
    
    private var poseDetector: PoseDetector? = nil
    var resultsText = ""
    
    var pointsHand = [CGPoint]()
    
    var images = [UIImage]()
    
    var labelTime: UILabel!
    var timer : Timer?
    var counter = 0
    let container: UIView = UIView()
    
    var scaleWidth: CGFloat = 0
    var scaleHeight: CGFloat = 0
    
    var timestamp: Double = 0
    
    private lazy var annotationOverlayView: UIView = {
        precondition(isViewLoaded)
        let annotationOverlayView = UIView(frame: .zero)
        annotationOverlayView.translatesAutoresizingMaskIntoConstraints = false
        annotationOverlayView.clipsToBounds = true
        return annotationOverlayView
    }()
    
    private lazy var handOverlayView: UIView = {
        precondition(isViewLoaded)
        let annotationOverlayView = UIView(frame: .zero)
        annotationOverlayView.translatesAutoresizingMaskIntoConstraints = false
        annotationOverlayView.clipsToBounds = true
        return annotationOverlayView
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        //        drawLine()
        //        drawLineAnimation()
        
        let options = AccuratePoseDetectorOptions()
        //        options.detectorMode = .singleImage
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
        videoPickerController.videoMaximumDuration = TimeInterval(240)
        videoPickerController.mediaTypes = [kUTTypeMovie as String]
        videoPickerController.modalPresentationStyle = .custom
        self.present(videoPickerController, animated: true, completion: nil)
        
    }
    
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.destination is ImagesViewController {
            let vc = segue.destination as! ImagesViewController
            vc.images.append(contentsOf: images)
        }
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
//        self.avplayer.rate = 4
        
//        video_vw.addSubview(annotationOverlayView)
//        NSLayoutConstraint.activate([
//            annotationOverlayView.topAnchor.constraint(equalTo: video_vw.topAnchor),
//            annotationOverlayView.leadingAnchor.constraint(equalTo: video_vw.leadingAnchor),
//            annotationOverlayView.trailingAnchor.constraint(equalTo: video_vw.trailingAnchor),
//            annotationOverlayView.bottomAnchor.constraint(equalTo: video_vw.bottomAnchor),
//        ])
        
    }
    
    func playVideo(videoURL: URL) {
        let playerLayer = AVPlayerLayer()
        video_vw.layer.addSublayer(playerLayer)
        playerLayer.frame = video_vw.bounds

        let asset = AVAsset(url: videoURL)
        let duration = asset.duration
        let seconds = CMTimeGetSeconds(duration)
        print("timeVideo: \(seconds)")
        let item = AVPlayerItem(asset: asset)
        let player = AVPlayer(playerItem: item)
        print("timeVideo: \(item.duration.seconds)")

        // 3
        player.actionAtItemEnd = .none

        // 4
        player.volume = 1.0
        player.rate = 4.0

        playerLayer.player = player
    }
    
    func drawLine() {
        let path = UIBezierPath()
        let fillColor = UIColor.blue
        
        let y: CGFloat = UIScreen.main.bounds.size.height
        let x: CGFloat = UIScreen.main.bounds.size.width
        let height: CGFloat = 200
        
        path.move(to: CGPoint(x: 0, y: y)) // bottom left
        path.addLine(to: CGPoint(x: 0, y: y - 20)) // top left
        path.addCurve(to: CGPoint(x: x, y: y - height), controlPoint1: CGPoint(x: x * 2 / 3, y: y), controlPoint2: CGPoint(x: x * 5 / 6, y: y - height * 6 / 5)) // curve to top right
        path.addLine(to: CGPoint(x: x, y: y)) // bottom right
        path.close() // close the path from bottom right to bottom left
        
        let shapeLayer = CAShapeLayer()
        shapeLayer.path = path.cgPath
        shapeLayer.fillColor = UIColor.clear.cgColor
        shapeLayer.strokeColor = fillColor.cgColor
        
        view.layer.addSublayer(shapeLayer)
    }
    
    func drawLineAnimation() {
        let path = UIBezierPath()
        path.move(to: CGPoint(x: 10, y: 130))
        path.addCurve(to: CGPoint(x: 100, y: 500), controlPoint1: CGPoint(x: 150, y: -100), controlPoint2: CGPoint(x: 100, y: 350))
        
        // create shape layer for that path (this defines what the path looks like when the animation is done)
        
        let shapeLayer = CAShapeLayer()
        shapeLayer.fillColor = #colorLiteral(red: 0, green: 0, blue: 0, alpha: 0).cgColor
        shapeLayer.strokeColor = #colorLiteral(red: 1, green: 0, blue: 0, alpha: 1).cgColor
        shapeLayer.lineWidth = 5
        shapeLayer.path = path.cgPath
        shapeLayer.strokeStart = 0.8
        
        //        let startAnimation = CABasicAnimation(keyPath: "strokeStart")
        //        startAnimation.fromValue = 0
        //        startAnimation.toValue = 0.8
        //
        //        let endAnimation = CABasicAnimation(keyPath: "strokeEnd")
        //        endAnimation.fromValue = 0.2
        //        endAnimation.toValue = 1.0
        //
        //        let animation = CAAnimationGroup()
        //        animation.animations = [startAnimation, endAnimation]
        //        animation.duration = 2
        //        shapeLayer.add(animation, forKey: "MyAnimation")
        view.layer.addSublayer(shapeLayer)
    }
    
    let data = [CGPoint(x: 60, y: 150), CGPoint(x: 140, y: 300),CGPoint(x: 230, y: 140), CGPoint(x: 310, y: 250), CGPoint(x: 390, y: 100), CGPoint(x: 490, y: 200), CGPoint(x: 580, y: 270),CGPoint(x: 650, y: 30) ]
    
    func drawHand(data: [CGPoint], color: CGColor) {
        //        let path = UIBezierPath()
        //        path.contractionFactor = CGFloat(0.7)
        //        path.move(to: pointsHand[1])
        //        path.addBezierThrough(points: pointsHand)
        
//        createPoints()
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
        video_vw.layer.addSublayer(_shapeLayer)
    }
    
    func createPoints() {
        for point in pointsHand {
            let circleLayer = CAShapeLayer()
            circleLayer.bounds = CGRect(x: 0, y: 0, width: 10, height: 10)
            circleLayer.path = UIBezierPath(ovalIn: circleLayer.bounds).cgPath
            circleLayer.fillColor = UIColor.red.cgColor
            circleLayer.position = point
            view.layer.addSublayer(circleLayer)
        }
    }
}

extension PlayerViewController: UIImagePickerControllerDelegate, UINavigationControllerDelegate, UIViewControllerTransitioningDelegate {
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [UIImagePickerController.InfoKey : Any]) {
        let videoURL = info[UIImagePickerController.InfoKey.mediaURL] as? URL
        print(videoURL?.path ?? "")
        self.dismiss(animated: true, completion: nil)
        
        if let videourl = videoURL {
//                                    self.addVideoPlayer(videoUrl: videourl, to: self.video_vw)
            loadAsset(url: videourl)
//            loadSlowMotion(url: videourl)
//            playVideo(videoURL: videourl)
        }
    }
}

extension PlayerViewController {
    private func loadAsset(url: URL) {
        timestamp = CACurrentMediaTime()
        DispatchQueue.main.async {
            self.showActivityIndicatory()
        }
        //        DispatchQueue.main.async {
        sessionQueue.async {
            let asset = AVAsset(url: url)
            if asset.metadata.count > 0 {
                for item in asset.metadata {
                    print("meta: \(item.identifier): \(item.value)")
                }
            }
            if let videoTrack = asset.tracks(withMediaType: .video).first {
               let videoFormatDescription = videoTrack.formatDescriptions.first as! CMVideoFormatDescription
                                        
                print (videoFormatDescription)

            }
            
            print("orientation: \(asset.videoOrientation().orientation), \(asset.videoOrientation().device)")
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
                    //                    if time < 10000 {
                    let imageWidth = CGFloat(CVPixelBufferGetWidth(imageBuffer))
                    let imageHeight = CGFloat(CVPixelBufferGetHeight(imageBuffer))

//                    self.images.append(image)

                    //                                            guard let newImage = self.updateImageView(with: image) else { return }

                    //                    let orientation = UIUtilities.imageOrientation(
                    //                        fromDevicePosition: asset.videoOrientation().device
                    //                    )
                    //                    guard let pixelBuffer = self.rotate(sampleBuffer) else {
                    //                        return
                    //                    }
//                    let image = self.createImage(imageBuffer: imageBuffer)
                    guard let  inputImage = MLImage(sampleBuffer: sampleBuffer) else {
                        print("Failed to create MLImage from UIImage.")
                        return
                    }
                    self.detectMovement(in: inputImage, width: imageWidth, height: imageHeight)

                }
            }
            
            if reader.status == AVAssetReader.Status.completed {
                DispatchQueue.main.async { [self] in
                    var time = CACurrentMediaTime() - timestamp
                    print("timestamp: \(time)")
                    let label = UILabel(frame: CGRect(x: 0, y: 0, width: 300, height: 21))
                    label.center = CGPoint(x: 160, y: 285)
                    label.textAlignment = .center
                    label.font = UIFont.preferredFont(forTextStyle: .title1)
                    label.textColor = .red
                    label.text = "\(time)s"
//                    view.addSubview(label)
                    self.addVideoPlayer(videoUrl: url, to: self.video_vw)
//                    self.img.image = image

                    drawHand(data: pointsHand, color: UIColor.red.cgColor)

                    var data1 = [CGPoint]()
                    var data2 = [CGPoint]()
                    var data3 = [CGPoint]()
                    var data4 = [CGPoint]()
                    for i in 0..<pointsHand.count {
                        if (i > 80 && i < 110 ) {
                            data1.append(pointsHand[i])
                        }
                        if (i > 124 && i < 129) {
                            data2.append(pointsHand[i])
                        }

//                        if (i > 137 && i < 142) {
//                            data3.append(pointsHand[i])
//                        }

                        if (i > 132 && i < 136) {
                            data3.append(pointsHand[i])
                        }

                        if (i > 175 && i < 190) {
                            data4.append(pointsHand[i])
                        }
                    }
                    drawHand(data: data1, color: UIColor.yellow.cgColor)
                    drawHand(data: data2, color: UIColor.green.cgColor)
                    drawHand(data: data3, color: UIColor.blue.cgColor)
                    drawHand(data: data4, color: UIColor.black.cgColor)
                    hideActivityIndicatory()
                }

            }
        }
        
        
    }
    
    func temporaryURL() -> URL? {
        let directory = try! FileManager.default.url(for:.documentDirectory, in: .userDomainMask, appropriateFor: nil, create: true)
        
        let videoURL = directory.appendingPathComponent(NSUUID().uuidString + ".mp4")
        return videoURL
    }
    
    // change orientation
    func loadSlowMotion(url: URL) {
        sessionQueue.async {
            let asset = AVAsset(url: url)
            let videoComposition = AVMutableVideoComposition(propertiesOf: asset)
            print("frame: \(videoComposition.frameDuration)")
            videoComposition.frameDuration = CMTime(seconds: 30, preferredTimescale: 1) // Changes FPS to 30
                                    
//            AVAssetExportPresetHighestQuality
            let exportSession = AVAssetExportSession(asset: asset, presetName: AVAssetExportPresetHighestQuality)

            exportSession?.videoComposition = videoComposition

            exportSession?.outputURL = self.temporaryURL()
            exportSession?.outputFileType = AVFileType.mov
            exportSession?.exportAsynchronously(completionHandler: {
                if let url = exportSession?.outputURL {
                    DispatchQueue.main.async {
//                        self.addVideoPlayer(videoUrl: url, to: self.video_vw)
                        self.loadAsset(url: url)
                        
                    }
                }
                
            })
        }
        
    }
    
    
    func createImage(imageBuffer: CVImageBuffer) -> UIImage {
        var image = UIImage()
        DispatchQueue.main.sync {
            let ciImage = CIImage(cvImageBuffer: imageBuffer).oriented(.right)
            let imagenew = UIImage(ciImage: ciImage)
            let imagen = self.updateImageView(with: imagenew)
            if imagen != nil {
                image = imagen!
            }
            
        }
        
        return image
    }
    
    func showActivityIndicatory() {
        container.frame = CGRect(x: UIScreen.main.bounds.width/2-40, y: 250, width: 80, height: 80) // Set X and Y whatever you want
        container.backgroundColor = .lightGray
        
        let activityView = UIActivityIndicatorView(style: UIActivityIndicatorView.Style.large)
        activityView.frame = CGRect(x: container.bounds.width/2-40, y: container.bounds.height/2-40, width: 80, height: 80)
        
        container.addSubview(activityView)
        self.view.addSubview(container)
        activityView.startAnimating()
        
        labelTime = UILabel(frame: CGRect(x: UIScreen.main.bounds.width/2-20, y: 335, width: 50, height: 21))
        //        labelTime.center = CGPoint(x: 160, y: 325)
        labelTime.textAlignment = .center
        labelTime.font = UIFont.preferredFont(forTextStyle: .title1)
        labelTime.textColor = .red
        labelTime.text = "0s"
        self.view.addSubview(labelTime)
        
        timer = Timer.scheduledTimer(timeInterval:1, target:self, selector:#selector(prozessTimer), userInfo: nil, repeats: true)
    }
    
    func hideActivityIndicatory() {
        container.isHidden = true
        labelTime.isHidden = true
    }
    
    @objc func prozessTimer() {
        counter += 1
        labelTime.text = "\(counter)s"
    }
    
    
    func configureVideo(videoURL: URL) {
        asset = AVURLAsset(url: videoURL)
        generator = AVAssetImageGenerator(asset: asset)
        generator.appliesPreferredTrackTransform = true
        generator.requestedTimeToleranceBefore = CMTimeMake(value: 1, timescale: 60000)
        generator.requestedTimeToleranceAfter = CMTimeMake(value: 1, timescale: 60000)
        avplayer = AVPlayer(url: videoURL)
        NotificationCenter.default.addObserver(self, selector:#selector(self.playerDidFinishPlaying(note:)),name: NSNotification.Name.AVPlayerItemDidPlayToEndTime, object: avplayer.currentItem)
        
        playerController.player = avplayer
        guard avplayer.currentItem?.asset != nil else {
            return
        }
        self.getVideoData()
        
    }
    
    @objc func playerDidFinishPlaying(note: NSNotification){
        print("avplayer: end")
        let asset = self.avplayer.currentItem?.asset
        let tracks = asset!.tracks(withMediaType: .video)
        let fps = tracks.first?.nominalFrameRate
        
        self.videoFPS = lround(Double(fps!))
        
        print("fps: \(String(describing: fps))")
        //            drawHand()
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
        
        let interval = CMTimeMake(value: 1 ,timescale: 60000)
        var dem: Int = 0
        
        //        let interval = CMTimeMakeWithSeconds(0.05 ,preferredTimescale: Int32(self.videoFPS))
        
        
        self.avplayer.addPeriodicTimeObserver(forInterval: interval, queue: DispatchQueue.main) {[weak self] (progressTime) in
            dem+=1
            print("time: \(dem)")
            //                                    self!.genetorImage()
            
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
            
            let currentTime: CMTime = self.avplayer.currentTime()
            let currentTimeInSecs: Float64 = CMTimeGetSeconds(currentTime)
            //            let actionTime: CMTime = CMTimeMake(value: Int64(currentTimeInSecs), timescale: 600)
            let actionTime: CMTime = CMTimeMakeWithSeconds(currentTimeInSecs, preferredTimescale: 600)
            
            print("currenttime: \(actionTime)")
            let imageRef = try self.generator.copyCGImage(at: currentTime, actualTime: nil)
            let image = UIImage(cgImage: imageRef)
            images.append(image)
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
                
                DispatchQueue.main.async{
                    //                let transform = self.transformMatrix(image: image)
                    let transform = self.transformMatrix(width: image.size.width, height: image.size.height)
                    
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
    }
    
    private func detectMovement(in image: MLImage, width: CGFloat, height: CGFloat) {
        //        self.poseDetector = nil
        //        let options = AccuratePoseDetectorOptions()
        //        self.poseDetector = PoseDetector.poseDetector(options: options)
        //        image.orientation = .left
        
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
                let transform1 = self.transformMatrix(width: scaleHeight, height: scaleWidth)

                
                poses.forEach { pose in
                    
                    let handOverlayView = UIUtilities.createMoveHandOverlayView(
                        fromPoint: strongSelf.pointsHand.last,
                        forPose: pose,
                        inViewWithBounds: strongSelf.handOverlayView.bounds,
                        lineWidth: Constants.lineWidthBold,
                        dotRadius: Constants.smallDotRadius,
                        positionTransformationClosure: { (position) -> CGPoint in
                            let guide = self.view.safeAreaLayoutGuide
                            let point = strongSelf.pointFrom(position).applying(transform)
                            self.pointsHand.append(point)
                            return point
                        })
//                    strongSelf.handOverlayView.addSubview(handOverlayView)
                    strongSelf.resultsText = "Pose Detected"
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
        return scaledImage
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
    
    private func transformMatrix(width: CGFloat, height: CGFloat) -> CGAffineTransform {
        //        let imageViewWidth = video_vw.frame.size.width
        //        let imageViewHeight = video_vw.frame.size.height
        //        let imageWidth = image.size.width
        //        let imageHeight = image.size.height
        
        let guide = view.safeAreaLayoutGuide
        let imageViewWidth = guide.layoutFrame.size.width
        let imageViewHeight = guide.layoutFrame.size.height
        
        let imageWidth = width
        let imageHeight = height
        
        let imageViewAspectRatio = imageViewWidth / imageViewHeight
        let imageAspectRatio = imageWidth / imageHeight
        let scale = (imageViewAspectRatio > imageAspectRatio) ? imageViewHeight / imageHeight : imageViewWidth / imageWidth
        
        // Image view's `contentMode` is `scaleAspectFit`, which scales the image to fit the size of the
        // image view by maintaining the aspect ratio. Multiple by `scale` to get image's original size.
        let scaledImageWidth = imageWidth * scale
        let scaledImageHeight = imageHeight * scale
        if scaleWidth == 0 {
        scaleWidth = scaledImageWidth
        scaleHeight = scaledImageHeight
        }
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
    static let lineWidthBold: CGFloat = 3.0
    static let fillColor = UIColor.clear.cgColor
    static let segmentationMaskAlpha: CGFloat = 0.5
}
