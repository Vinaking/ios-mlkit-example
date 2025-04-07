//
//  CropViewController.swift
//  MLKitDemo
//
//  Created by Tùng on 03/11/2022.
//

import UIKit
import PhotosUI

class CropViewController: UIViewController {
    @IBOutlet private var segment: UISegmentedControl!
    @IBOutlet private var videoView: UIView!
    @IBOutlet private var viewCorner: UIView!
    @IBOutlet private var viewCorner1: UIView!
    @IBOutlet private var imageView: UIImageView!
    private var player: AVPlayer!
    let playerLayer = AVPlayerLayer()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        segment.removeBorders()
        segment.setImage(
            UIImage.textEmbededImage(
                image: UIImage(named: "Image")!,
                string: "First",
                color: .black
            ),
            forSegmentAt: 0
        )
        segment.setImage(
            UIImage.textEmbededImage(
                image: UIImage(named: "Image")!,
                string: "Second",
                color: .black
            ),
            forSegmentAt: 1
        )
        
        viewCorner.roundCorners([.topRight, .bottomRight], radius: 10)
        viewCorner1.roundCorners([.topLeft, .bottomLeft], radius: 10)
        //viewCorner1.backgroundColor = UIColor.gray
        
        let points: [CGPoint] = [
            CGPoint(x: 150, y: 300),
            CGPoint(x: 310, y: 510),
            CGPoint(x: 100, y: 510)
        ]
        
        drawLine(view: videoView, data: points, color: UIColor.green.cgColor)
        //        createPoints(points: points)
        
        let point1s: [CGPoint] = [
            CGPoint(x: 310, y: 310),
            CGPoint(x: 300, y: 300)
        ]
        
        drawLine(view: videoView, data: point1s, color: UIColor.green.cgColor)
        //        createPoints(points: point1s)
        
        imageView.image = imageView.image?.withRenderingMode(.alwaysTemplate)
        imageView.tintColor = UIColor.orange
        
    }
    
    @IBAction func btn1Click(_ sender: Any) {
        viewCorner1.backgroundColor = UIColor.purple
        viewCorner.backgroundColor = UIColor.gray
    }
    
    @IBAction func btn2Click(_ sender: Any) {
        viewCorner1.backgroundColor = UIColor.gray
        viewCorner.backgroundColor = UIColor.purple
    }
    
    @IBAction func segmentControllClick(_ sender: Any) {
        switch segment.selectedSegmentIndex {
        case 0:
            print("First")
        case 1 :
            print("Second")
        default:
            break
        }
        
        
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
        configuration.selectionLimit = 0
        // Set the preselected asset identifiers with the identifiers that the app tracks.
        //        configuration.preselectedAssetIdentifiers = selectedAssetIdentifiers
        
        let picker = PHPickerViewController(configuration: configuration)
        picker.delegate = self
        present(picker, animated: true)
        
    }
    
    private func drawLine(view: UIView, data: [CGPoint], color: CGColor) {
        
        let path = UIBezierPath()
        let config = BezierConfiguration()
        let controlPoints = config.configureControlPoints(data: data)
        for i in 0..<data.count {
            let point = data[i]
            if i == 0 {
                path.move(to: point)
            }else {
                let segment = controlPoints[i - 1]
                path.addCurve(to: point, controlPoint1: segment.firstControlPoint, controlPoint2: segment.secondControlPoint)
            }
        }
        
//        let animation : CABasicAnimation = CABasicAnimation(keyPath: "strokeEnd")
//        animation.fromValue = 0.0
//        animation.toValue = 1.0
//
//        animation.duration = 7.0
        
        let shapeLayer = CAShapeLayer()
        
        shapeLayer.strokeColor = color
        shapeLayer.fillColor = nil
        shapeLayer.lineWidth = 1.5
        shapeLayer.path = path.cgPath
        shapeLayer.lineCap = CAShapeLayerLineCap.round
        shapeLayer.isHidden = false
        shapeLayer.strokeEnd = 10
//        shapeLayer.add(animation, forKey: "myStroke")
//        CATransaction.commit()
        view.layer.addSublayer(shapeLayer)
        
        let l = 1 - (6/path.mx_length)
        let p = path.mx_point(atFractionOfLength: l)
        print("pointLog: \(p)")
        //        createPoints(points: [p])
        
        print("datarange: \(data.count)")
        let arrow = UIBezierPath.arrow(from: p, to: data[data.count-1],
                                       tailWidth: 0, headWidth: 7, headLength: 7)
        let shapeLayerArrow = CAShapeLayer()
        
        shapeLayerArrow.strokeColor = color
        shapeLayerArrow.fillColor = color
        shapeLayerArrow.lineWidth = 1.5
        shapeLayerArrow.path = arrow.cgPath
        shapeLayerArrow.lineCap = CAShapeLayerLineCap.round
        shapeLayerArrow.isHidden = false
        view.layer.addSublayer(shapeLayerArrow)
        
    }
    
    func createPoints(points: [CGPoint]) {
        for point in points {
            let circleLayer = CAShapeLayer()
            circleLayer.bounds = CGRect(x: 0, y: 0, width: 10, height: 10)
            circleLayer.path = UIBezierPath(ovalIn: circleLayer.bounds).cgPath
            circleLayer.fillColor = UIColor.red.cgColor
            circleLayer.position = point
            videoView.layer.addSublayer(circleLayer)
        }
    }
    
}

extension CropViewController: PHPickerViewControllerDelegate {
    func picker(_ picker: PHPickerViewController, didFinishPicking results: [PHPickerResult]) {
        picker.dismiss(animated: true, completion: nil)
        guard let provider = results.first?.itemProvider else { return }
        if provider.hasItemConformingToTypeIdentifier(UTType.movie.identifier) {
            provider.loadItem(forTypeIdentifier: UTType.movie.identifier, options: [:]) { (videoURL, error) in
                if let url = videoURL as? URL {
                    self.cropVideo(url: url, startTime: 1, endTime: 4)
                }
                
            }
        }
    }
    
    func cropVideo(url: URL, startTime: Float, endTime: Float) {
        let manager = FileManager.default
        guard let documentDirectory = try? manager.url(for: .documentDirectory, in: .userDomainMask, appropriateFor: nil, create: true) else {return}
        let asset = AVAsset(url: url as URL)
        let length = Float(asset.duration.value) / Float(asset.duration.timescale)
        print("video length: \(asset.duration) seconds")
    }
}

extension UISegmentedControl {
    func removeBorders() {
        setBackgroundImage(imageWithColor(color: backgroundColor ?? UIColor.gray), for: .normal, barMetrics: .default)
        setBackgroundImage(imageWithColor(color: UIColor.green), for: .selected, barMetrics: .default)
        setDividerImage(imageWithColor(color: UIColor.gray), forLeftSegmentState: .normal, rightSegmentState: .normal, barMetrics: .default)
    }
    
    // create a 1x1 image with this color
    private func imageWithColor(color: UIColor) -> UIImage {
        let rect = CGRect(x: 0.0, y: 0.0, width:  1.0, height: 1.0)
        UIGraphicsBeginImageContext(rect.size)
        let context = UIGraphicsGetCurrentContext()
        context!.setFillColor(color.cgColor);
        context!.fill(rect);
        let image = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        return image!
    }
}

extension UIImage {
    
    class func textEmbededImage(image: UIImage, string: String, color:UIColor, imageAlignment: Int = 0, segFont: UIFont? = nil) -> UIImage {
        let font = segFont ?? UIFont.systemFont(ofSize: 16.0)
        let expectedTextSize: CGSize = (string as NSString).size(withAttributes: [NSAttributedString.Key.font: font])
        let width: CGFloat = expectedTextSize.width + image.size.width + 5.0
        let height: CGFloat = max(expectedTextSize.height, image.size.width)
        let size: CGSize = CGSize(width: width, height: height)
        UIGraphicsBeginImageContextWithOptions(size, false, 0)
        let context: CGContext = UIGraphicsGetCurrentContext()!
        context.setFillColor(color.cgColor)
        let fontTopPosition: CGFloat = (height - expectedTextSize.height) / 2.0
        let textOrigin: CGFloat = (imageAlignment == 0) ? image.size.width + 5 : 0
        let textPoint: CGPoint = CGPoint.init(x: textOrigin, y: fontTopPosition)
        string.draw(at: textPoint, withAttributes: [NSAttributedString.Key.font: font])
        let flipVertical: CGAffineTransform = CGAffineTransform(a: 1, b: 0, c: 0, d: -1, tx: 0, ty: size.height)
        context.concatenate(flipVertical)
        let alignment: CGFloat =  (imageAlignment == 0) ? 0.0 : expectedTextSize.width + 5.0
        context.draw(image.cgImage!, in: CGRect.init(x: alignment, y: ((height - image.size.height) / 2.0), width: image.size.width, height: image.size.height))
        let newImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        
        return newImage!
    }
    
}

extension UIView {
    func roundCorners(_ corners:UIRectCorner, radius: CGFloat) {
        let path = UIBezierPath(roundedRect: self.bounds, byRoundingCorners: corners, cornerRadii: CGSize(width: radius, height: radius))
        let mask = CAShapeLayer()
        mask.path = path.cgPath
        self.layer.mask = mask
    }
}
