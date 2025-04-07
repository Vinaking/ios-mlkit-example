//
//  PickerVideoViewController.swift
//  MLKitDemo
//
//  Created by Tùng on 26/10/2022.
//

import UIKit
import PhotosUI
import mobileffmpeg
import CoreMotion

@available(iOS 14, *)
class PickerVideoViewController: UIViewController {
    
    var orientationLast = UIInterfaceOrientation(rawValue: 0)!
    var currentAgle: CGFloat = 0
    var motionManager: CMMotionManager?
    
    @IBOutlet private var topConstraint: NSLayoutConstraint!
    @IBOutlet private var videoView1: UIView!
    private var player1: AVPlayer!
    let playerLayer1 = AVPlayerLayer()
    
    var width: CGFloat = 0
    var height: CGFloat = 0
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        videoView1.layer.addSublayer(playerLayer1)
        playerLayer1.frame = videoView1.bounds
        
        //        self.navigationController?.navigationBar.barTintColor = UIColor.orange
        //        self.navigationController?.navigationBar.tintColor = UIColor.white
        //        self.navigationController?.navigationBar.titleTextAttributes = [NSAttributedString.Key.foregroundColor: UIColor.white]
        
        //        let appearance = UINavigationBarAppearance()
        //        appearance.configureWithOpaqueBackground()
        //        appearance.backgroundColor = UIColor.white
        //        appearance.titleTextAttributes = [NSAttributedString.Key.foregroundColor: UIColor.black]
        //        self.navigationController?.navigationBar.standardAppearance = appearance
        //        self.navigationController?.navigationBar.scrollEdgeAppearance = self.navigationController?.navigationBar.standardAppearance
        
        let window = UIApplication.shared.windows.filter {$0.isKeyWindow}.first
        let statusBarHeight = window?.windowScene?.statusBarManager?.statusBarFrame.height ?? 0
        print("height: \(statusBarHeight)")
        topConstraint.constant = statusBarHeight
        
        width = videoView1.frame.width
        height = videoView1.frame.height
//        initializeMotionManager()
        
        let child = ChildViewController()
        addchildViewController(asChildViewController: child)
    }
    
    override func viewWillAppear(_ animated: Bool) {
        
        navigationController?.navigationBar.setBackgroundImage(UIImage(), for: UIBarMetrics.default)
        navigationController?.navigationBar.backgroundColor = UIColor.clear
        navigationController?.navigationBar.shadowImage = UIImage()
        navigationController?.navigationBar.isTranslucent = true
        navigationController?.navigationBar.tintColor = .red
        
        let statusBar = UIView(frame: UIApplication.shared.windows.filter {$0.isKeyWindow}.first?.windowScene?.statusBarManager?.statusBarFrame ?? CGRect.zero)
        statusBar.backgroundColor = UIColor.green
        statusBar.tag = 100
        view.addSubview(statusBar)
        
        
    }
    
    public override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
       return UIInterfaceOrientationMask.portrait
   }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func photoPicker(_ sender: Any) {
        var configuration = PHPickerConfiguration(photoLibrary: .shared())
        
        // Set the filter type according to the user’s selection.
        configuration.filter = PHPickerFilter.videos
        // Set the mode to avoid transcoding, if possible, if your app supports arbitrary image/video encodings.
        configuration.preferredAssetRepresentationMode = .current
        // Set the selection behavior to respect the user’s selection order.
        //        configuration.selection = .ordered
        // Set the selection limit to enable multiselection.
        configuration.selectionLimit = 1
        // Set the preselected asset identifiers with the identifiers that the app tracks.
        //        configuration.preselectedAssetIdentifiers = selectedAssetIdentifiers
        
        let picker = PHPickerViewController(configuration: configuration)
        picker.delegate = self
        present(picker, animated: true)
        
    }
    
    func addchildViewController(asChildViewController viewController: UIViewController) {
        // Add Child View Controller
        addChild(viewController)
        
        // Add Child View as Subview
        videoView1.addSubview(viewController.view)
        
        // Configure Child View
        viewController.view.frame = videoView1.bounds
        viewController.view.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        
        // Notify Child View Controller
        viewController.didMove(toParent: self)
    }
    
}

@available(iOS 14, *)
extension PickerVideoViewController: PHPickerViewControllerDelegate {
    func picker(_ picker: PHPickerViewController, didFinishPicking results: [PHPickerResult]) {
        picker.dismiss(animated: true, completion: nil)
        guard let provider = results.first?.itemProvider else { return }
        if provider.hasItemConformingToTypeIdentifier(UTType.movie.identifier) {
            provider.loadItem(forTypeIdentifier: UTType.movie.identifier, options: [:]) { (videoURL, error) in
                let identifiers = results.compactMap(\.assetIdentifier)
                let phAsset = PHAsset.fetchAssets(withLocalIdentifiers: identifiers, options: nil).firstObject
                let videoOptions = PHVideoRequestOptions()
                videoOptions.version = PHVideoRequestOptionsVersion.original
                PHImageManager.default().requestAVAsset(forVideo: phAsset!, options: videoOptions , resultHandler: { (asset, audioMix, info) -> Void in
                    if let asset = asset as? AVURLAsset {
                        let videoData = NSData(contentsOf: asset.url)
                        let videoPath = NSTemporaryDirectory() + "tmpMovie.MOV"
                        let videoURL = NSURL(fileURLWithPath: videoPath)
                        let writeResult = videoData?.write(to: videoURL as URL, atomically: true)
                        
                        if let writeResult = writeResult, writeResult {
                            print("success")
                            
                            let a = AVAsset(url: videoURL as URL)
                            let item = AVPlayerItem(asset: a)
                            self.player1 = AVPlayer(playerItem: item)
                            print("timeVideo: \(item.duration.seconds)")
                            
                            // 3
                            self.player1.actionAtItemEnd = .none
                            
                            // 4
                            self.player1.volume = 1.0
                            self.player1.rate = 1.0
                            
                            self.playerLayer1.player = self.player1
                            
                            let tracks = asset.tracks(withMediaType: .video)
                            let fps = tracks.first?.nominalFrameRate
                            let ab = lround(Double(fps!))
                            print("fps: \(String(describing: ab))")
                            
                        }
                        else {
                            print("failure")
                        }
                    }
                })
                if let url = videoURL as? URL {
                    print("resullt: \(videoURL)")
                    let asset = AVAsset(url: url)
                    
                }
                
            }
        }
    }
    
    private func initializeMotionManager() {
        motionManager = CMMotionManager()
        motionManager?.accelerometerUpdateInterval = 0.2
        motionManager?.gyroUpdateInterval = 0.2
        motionManager?.startAccelerometerUpdates(to: (OperationQueue.current)!, withHandler: {
            (accelerometerData, error) -> Void in
            if error == nil {
                self.outputAccelertionData((accelerometerData?.acceleration)!)
            }
            else {
                print("\(error!)")
            }
        })
    }
    
    private func outputAccelertionData(_ acceleration: CMAcceleration) {
        var isLanscape = false
        var orientationNew: UIInterfaceOrientation
        if acceleration.x >= 0.75 {
            orientationNew = .landscapeLeft
            isLanscape = true
        }
        else if acceleration.x <= -0.75 {
            orientationNew = .landscapeRight
            isLanscape = true
        }
        else if acceleration.y <= -0.75 {
            orientationNew = .portrait
            isLanscape = false
            
        }
        else if acceleration.y >= 0.75 {
            orientationNew = .portraitUpsideDown
            isLanscape = false
        }
        else {
            // Consider same as last time
            return
        }
        
        if orientationNew == orientationLast {
            return
        }
        orientationLast = orientationNew
        
        setUpViewOrientation(isLanscape: isLanscape)
    }
    
    private func setUpViewOrientation(isLanscape: Bool) {
        videoView1.rotate(angle: -currentAgle, w: width, h: height)
        
        var angle: CGFloat = 0
        switch orientationLast {
        case .portrait:
            angle = 0
            break
        case .landscapeLeft:
            angle = -90
            break
        case .landscapeRight:
            angle = 90
            break
        case .portraitUpsideDown:
            angle = 180
            break
        case .unknown:
            break
        @unknown default:
            break
        }
        
        currentAgle = angle
        videoView1.rotate(angle: angle, w: width, h: height)
        videoView1.removeFromSuperview()
        videoView1.frame = CGRectMake(0, 0, 0, 0)
        videoView1.bounds = CGRectMake(0, 0, 0, 0)
        view.addSubview(videoView1)
        NSLayoutConstraint.activate([
            videoView1.topAnchor.constraint(equalTo: view.topAnchor),
            videoView1.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            videoView1.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            videoView1.bottomAnchor.constraint(equalTo: view.bottomAnchor),
        ])
        videoView1.layoutIfNeeded()
//        videoView1.updateConstraints()
    }
}

extension UIView {
    
    func rotate(angle: CGFloat, w: CGFloat, h: CGFloat) {
        
        let radians = angle / 180.0 * CGFloat.pi
        let rotation = CGAffineTransformRotate(self.transform, radians)

        transform = rotation
        
    }

}

extension UINavigationController {

open override var shouldAutorotate: Bool {
    return true
}

open override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
    return (visibleViewController?.supportedInterfaceOrientations)!
    }
}
