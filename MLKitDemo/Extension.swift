//
//  Extension.swift
//  MLKitDemo
//
//  Created by Tùng on 02/10/2022.
//

import Foundation
import CoreGraphics
import UIKit
import MLKit
import PhotosUI

extension UIImage {

  /// Creates and returns a new image scaled to the given size. The image preserves its original PNG
  /// or JPEG bitmap info.
  ///
  /// - Parameter size: The size to scale the image to.
  /// - Returns: The scaled image or `nil` if image could not be resized.
  public func scaledImage(with size: CGSize) -> UIImage? {
    UIGraphicsBeginImageContextWithOptions(size, false, scale)
    defer { UIGraphicsEndImageContext() }
    draw(in: CGRect(origin: .zero, size: size))
    return UIGraphicsGetImageFromCurrentImageContext()?.data.flatMap(UIImage.init)
  }

  // MARK: - Private

  /// The PNG or JPEG data representation of the image or `nil` if the conversion failed.
  private var data: Data? {
    #if swift(>=4.2)
      return self.pngData() ?? self.jpegData(compressionQuality: Constant.jpegCompressionQuality)
    #else
      return self.pngData() ?? self.jpegData(compressionQuality: Constant.jpegCompressionQuality)
    #endif  // swift(>=4.2)
  }
}

private enum Constant {
  static let jpegCompressionQuality: CGFloat = 0.8
}

extension UIImage {
    func rotate(radians: Float) -> UIImage {
//        DispatchQueue.main.sync {
            var newSize = CGRect(origin: CGPoint.zero, size: self.size).applying(CGAffineTransform(rotationAngle: CGFloat(radians))).size
            // Trim off the extremely small float value to prevent core graphics from rounding it up
            newSize.width = floor(newSize.width)
            newSize.height = floor(newSize.height)

            UIGraphicsBeginImageContextWithOptions(newSize, false, self.scale)
            let context = UIGraphicsGetCurrentContext()!

            // Move origin to middle
            context.translateBy(x: newSize.width/2, y: newSize.height/2)
            // Rotate around middle
            context.rotate(by: CGFloat(radians))
            // Draw the image at its center
            self.draw(in: CGRect(x: -self.size.width/2, y: -self.size.height/2, width: self.size.width, height: self.size.height))

            let newImage = UIGraphicsGetImageFromCurrentImageContext()
            UIGraphicsEndImageContext()

            return newImage!
//        }
        
    }
}

extension CGPoint {
    func scale(guide: UILayoutGuide, width: CGFloat, height: CGFloat) -> CGPoint {
        let imageViewWidth = guide.layoutFrame.size.width
        let imageViewHeight = guide.layoutFrame.size.height
        
        let imageWidth = width
        let imageHeight = height
        
        let imageViewAspectRatio = imageViewWidth / imageViewHeight
        let imageAspectRatio = imageWidth / imageHeight
        let scale = (imageViewAspectRatio > imageAspectRatio) ? imageViewHeight / imageHeight : imageViewWidth / imageWidth
        
        return CGPoint(x: self.x / 1.04242391, y: self.y / 1.004595805)
    }
}


