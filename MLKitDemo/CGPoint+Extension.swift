//
//  CGPoint+Extension.swift
//  MLKitDemo
//
//  Created by Tùng on 15/10/2022.
//

import Foundation
import UIKit

extension CGPoint {
    func rotate(around center: CGPoint, angle: CGFloat) -> CGPoint {
        let translate = CGAffineTransform(translationX: -center.x, y: -center.y)
        let transform = translate.concatenating(CGAffineTransform(rotationAngle: angle))
        let rotated = applying(transform)
        return rotated.applying(CGAffineTransform(translationX: center.x, y: center.y))
    }
}
