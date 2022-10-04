//
//  Extension.swift
//  MLKitDemo
//
//  Created by Tùng on 02/10/2022.
//

import Foundation
import CoreGraphics
import UIKit

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
