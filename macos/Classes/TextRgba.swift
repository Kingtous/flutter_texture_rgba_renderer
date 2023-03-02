//
//  TextRgba.swift
//  texture_rgba_renderer
//
//  Created by kingtous on 2023/2/17.
//

import Foundation
import FlutterMacOS
import CoreVideo

@objc public class TextRgba: NSObject, FlutterTexture {
    public var textureId: Int64 = -1
    private var registry: FlutterTextureRegistry?
    private var data: CVPixelBuffer?
    private var width: Int = 0
    private var height: Int = 0
    private let queue = DispatchQueue(label: "text_rgba_sync_queue")
    
    public static func new(registry: FlutterTextureRegistry?) -> TextRgba {
        let textRgba = TextRgba()
        textRgba.registry = registry
        textRgba.textureId = registry?.register(textRgba) ?? -1
        return textRgba
    }
    
    public func copyPixelBuffer() -> Unmanaged<CVPixelBuffer>? {
        queue.sync {
            if (data == nil) {
                return nil
            }
            return Unmanaged.passRetained(data!)
        }
    }
    
    
    @objc public func markFrameAvaliable(data: Data, width: Int, height: Int, row_align_bytes: Int) -> Bool {
        queue.sync {
            self.data = data.withUnsafeBytes { buffer in
                var pixelBufferCopy: CVPixelBuffer!
                // macOS only support 32BGRA currently.
                let result = CVPixelBufferCreate(kCFAllocatorDefault, width, height, kCVPixelFormatType_32BGRA, [
                    kCVPixelBufferPixelFormatTypeKey: kCVPixelFormatType_32BGRA,
                    kCVPixelBufferMetalCompatibilityKey: true,
                    kCVPixelBufferOpenGLCompatibilityKey: true,
                    // https://developer.apple.com/forums/thread/712709
                    kCVPixelBufferBytesPerRowAlignmentKey: row_align_bytes
                ] as CFDictionary, &pixelBufferCopy)
                guard result == kCVReturnSuccess else {
                    return nil
                }
                CVPixelBufferLockBaseAddress(pixelBufferCopy, [])
                let source = buffer.baseAddress!
                let ptr = CVPixelBufferGetBaseAddress(pixelBufferCopy!)!
                memcpy(ptr, source, data.count)
                CVPixelBufferUnlockBaseAddress(pixelBufferCopy, [])
                return pixelBufferCopy
            }
            self.width = width
            self.height = height
            if (textureId != -1 && self.data != nil) {
                registry?.textureFrameAvailable(textureId)
                return true
            } else {
                return false
            }
        }
    }
}


