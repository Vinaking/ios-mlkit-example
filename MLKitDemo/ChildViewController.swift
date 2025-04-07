//
//  ChildViewController.swift
//  MLKitDemo
//
//  Created by Tùng on 20/11/2022.
//

import UIKit

class ChildViewController: UIViewController {
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Do any additional setup after loading the view.
        
//        UIDevice.current.setValue(UIInterfaceOrientation.landscapeLeft.rawValue, forKey: "orientation")
        (UIApplication.shared.delegate as! AppDelegate).deviceOrientation = .all
    }
    
    public override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
        return UIInterfaceOrientationMask.portrait
    }
    
}
