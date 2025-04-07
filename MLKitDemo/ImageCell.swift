//
//  ImageCell.swift
//  MLKitDemo
//
//  Created by Tùng on 07/10/2022.
//

import UIKit

class ImageCell: UICollectionViewCell {
    
    @IBOutlet private var imageView: UIImageView!
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }
    
    var image: UIImage? {
        didSet {
            imageView.image = image
        }
    }

}
