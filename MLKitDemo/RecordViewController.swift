//
//  RecordViewController.swift
//  MLKitDemo
//
//  Created by Tùng on 01/03/2023.
//

import UIKit

class RecordViewController: UIViewController, AVCaptureVideoDataOutputSampleBufferDelegate, AVCaptureAudioDataOutputSampleBufferDelegate {
    
    @IBOutlet private weak var cameraView: UIView!
    private var previewLayer: AVCaptureVideoPreviewLayer!
    private lazy var captureSession = AVCaptureSession()
    private lazy var sessionQueue = DispatchQueue(label: "Session Queue")
    private var videoOutput: AVCaptureVideoDataOutput?
    private var audioOutput: AVCaptureAudioDataOutput?
    
    private var isUsingFrontCamera = false
    
    private enum CaptureState {
        case idle, start, capturing, end
    }
    
    private var captureState = CaptureState.idle

    override func viewDidLoad() {
        super.viewDidLoad()

        previewLayer = AVCaptureVideoPreviewLayer(session: captureSession)
        setUpCaptureSessionOutput()
        setUpCaptureSessionInput()
        
        previewLayer.frame = cameraView.bounds
        previewLayer.videoGravity = AVLayerVideoGravity.resizeAspectFill
        cameraView.layer.addSublayer(previewLayer)
    }
    

    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        
        startSession()
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        
        stopSession()
        
    }
    
    private func startSession() {
        weak var weakSelf = self
        sessionQueue.async {
            guard let strongSelf = weakSelf else {
                print("Self is nil!")
                return
            }
            strongSelf.captureSession.startRunning()
            strongSelf.captureSession.sessionPreset = .inputPriority
        }
    }
    
    private func stopSession() {
        weak var weakSelf = self
        sessionQueue.async {
            guard let strongSelf = weakSelf else {
                print("Self is nil!")
                return
            }
            strongSelf.captureSession.stopRunning()
        }
    }
    
    private func setUpCaptureSessionInput() {
        weak var weakSelf = self
        sessionQueue.async {
            guard let strongSelf = weakSelf else {
                print("Self is nil!")
                return
            }
            let cameraPosition: AVCaptureDevice.Position = strongSelf.isUsingFrontCamera ? .front : .back
            guard let device = strongSelf.captureDevice(forPosition: cameraPosition) else {
                print("Failed to get capture device for camera position: \(cameraPosition)")
                return
            }
            
            //            guard let audio = strongSelf.captureAudio() else {
            //                print("Failed to get capture device for camera position: \(cameraPosition)")
            //                return
            //            }
            
            let audioDevice = AVCaptureDevice.default(for: .audio)
            
            do {
                strongSelf.captureSession.beginConfiguration()
                let currentInputs = strongSelf.captureSession.inputs
                for input in currentInputs {
                    strongSelf.captureSession.removeInput(input)
                }
                
                let input = try AVCaptureDeviceInput(device: device)
                guard strongSelf.captureSession.canAddInput(input) else {
                    print("Failed to add capture session input.")
                    return
                }
                
//                let audioInput = try AVCaptureDeviceInput(device: audioDevice!)
//                guard strongSelf.captureSession.canAddInput(audioInput) else {
//                    print("Failed to add capture session audio input.")
//                    return
//                }
                
                strongSelf.captureSession.addInput(input)
//                strongSelf.captureSession.addInput(audioInput)
                strongSelf.captureSession.commitConfiguration()
            } catch {
                print("Failed to create capture device input: \(error.localizedDescription)")
            }
            
            
            var bestFormat: AVCaptureDevice.Format? = nil
            var bestFrameRateRange: AVFrameRateRange? = nil
            var bestPixelArea: Int32 = 0
            for format in device.formats {
                let dims: CMVideoDimensions = CMVideoFormatDescriptionGetDimensions(format.formatDescription)
                let pixelArea: Int32 = dims.width * dims.height
                let ranges = format.videoSupportedFrameRateRanges
                for range in ranges {
                    if range.maxFrameRate <= 120 {
                        if bestFrameRateRange==nil || range.maxFrameRate > bestFrameRateRange!.maxFrameRate || ((range.maxFrameRate == bestFrameRateRange!.maxFrameRate) && (pixelArea > bestPixelArea)) {
                            bestFormat = format
                            bestFrameRateRange = range
                            bestPixelArea = pixelArea
                        }
                    }
                }
            }

            do {
                try device.lockForConfiguration()
                device.activeFormat = bestFormat!
                device.activeVideoMinFrameDuration = bestFrameRateRange!.minFrameDuration
                device.activeVideoMaxFrameDuration = bestFrameRateRange!.minFrameDuration
            }
            catch{}
            device.unlockForConfiguration()
        }
    }
    
    private func setUpCaptureSessionOutput() {
        weak var weakSelf = self
        sessionQueue.async {
            guard let strongSelf = weakSelf else {
                print("Self is nil!")
                return
            }
            strongSelf.captureSession.beginConfiguration()
            // When performing latency tests to determine ideal capture settings,
            // run the app in 'release' mode to get accurate performance metrics
            strongSelf.captureSession.sessionPreset = AVCaptureSession.Preset.inputPriority
            
            let output = AVCaptureVideoDataOutput()
            output.videoSettings = [
                (kCVPixelBufferPixelFormatTypeKey as String): kCVPixelFormatType_32BGRA
            ]
            output.alwaysDiscardsLateVideoFrames = true
            //            let outputQueue = DispatchQueue(label: Constant.videoDataOutputQueueLabel)
            let outputQueue = DispatchQueue(label: "com.golf.swing")
            output.setSampleBufferDelegate(strongSelf, queue: outputQueue)
            guard strongSelf.captureSession.canAddOutput(output) else {
                print("Failed to add capture session output.")
                return
            }
            
            let audioOutput = AVCaptureAudioDataOutput()
            audioOutput.setSampleBufferDelegate(strongSelf, queue: outputQueue)
            
            strongSelf.audioOutput = audioOutput
            strongSelf.videoOutput = output
            strongSelf.captureSession.addOutput(audioOutput)
            strongSelf.captureSession.addOutput(output)
            strongSelf.captureSession.commitConfiguration()
            
            
            //            guard strongSelf.captureSession.canAddOutput(self.movieOutput) else {
            //                print("Failed to add capture session output.")
            //                return
            //            }
            //            strongSelf.captureSession.addOutput(self.movieOutput)
            //            strongSelf.captureSession.commitConfiguration()
        }
    }
    
    private func captureDevice(forPosition position: AVCaptureDevice.Position) -> AVCaptureDevice? {
        if #available(iOS 10.0, *) {
            let discoverySession = AVCaptureDevice.DiscoverySession(
                deviceTypes: [.builtInWideAngleCamera],
                mediaType: .video,
                position: .unspecified
            )
            return discoverySession.devices.first { $0.position == position }
        }
        return nil
    }
    
    private func captureAudio() -> AVCaptureDevice? {
        if #available(iOS 10.0, *) {
            let discoverySession = AVCaptureDevice.DiscoverySession(
                deviceTypes: [.builtInMicrophone],
                mediaType: .audio,
                position: .unspecified
            )
            for device in discoverySession.devices {
                if device.hasMediaType(.audio) {
                    return device
                }
            }
        }
        return nil
    }

}
