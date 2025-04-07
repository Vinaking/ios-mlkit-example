//
//  ImagesViewController.swift
//  MLKitDemo
//
//  Created by Tùng on 07/10/2022.
//

import UIKit

class ImagesViewController: UIViewController {
    @IBOutlet private var collectionView: UICollectionView!
    
    var images: [UIImage] = []
    override func viewDidLoad() {
        super.viewDidLoad()

        collectionView.register(UINib(nibName: "ImageCell", bundle: nil), forCellWithReuseIdentifier: "ImageCell")
        let width = (UIScreen.main.bounds.width - 8) / 3
        let layout = collectionView.collectionViewLayout as! UICollectionViewFlowLayout
        layout.itemSize = CGSize(width: width, height: width)
        layout.sectionHeadersPinToVisibleBounds = true
    }

}

extension ImagesViewController: UICollectionViewDataSource, UICollectionViewDelegate {
    
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        print("count: \(images.count)")
        return images.count
    }
    
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "ImageCell", for: indexPath) as! ImageCell
        cell.image = images[indexPath.row]
        return cell
    }
    
    
}
