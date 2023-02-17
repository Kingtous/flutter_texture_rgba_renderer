//
//  TextRgba.swift
//  texture_rgba_renderer
//
//  Created by kingtous on 2023/2/17.
//

import Foundation
import FlutterMacOS
import CoreVideo

public final class TextRgba: NSObject, FlutterTexture {
    public var textureId: Int64 = -1
    private var registry: FlutterTextureRegistry?
    private var data: CVPixelBuffer?
    private var width: Int = 0
    private var height: Int = 0
    
    public static func new(registry: FlutterTextureRegistry?) -> TextRgba {
        let textRgba = TextRgba()
        textRgba.registry = registry
        textRgba.textureId = registry?.register(textRgba) ?? -1
        return textRgba
    }
    
    public func copyPixelBuffer() -> Unmanaged<CVPixelBuffer>? {
        if (data == nil) {
            return nil
        }
        return Unmanaged.passUnretained(data!)
    }
    
    public func markFrameAvaliable(data: Data, width: Int, height: Int) -> Bool {
        self.data = data.withUnsafeBytes { buffer in
            var pixelBufferCopy: CVPixelBuffer? = nil
            CVPixelBufferCreate(kCFAllocatorDefault, width, height, kCVPixelFormatType_32RGBA, nil, &pixelBufferCopy)
            if (pixelBufferCopy == nil) {
                return nil
            }
            CVPixelBufferLockBaseAddress(pixelBufferCopy!, [])
            defer {CVPixelBufferUnlockBaseAddress(pixelBufferCopy!, [])}
            var source = buffer.baseAddress!
            
            for plane in 0 ..< CVPixelBufferGetPlaneCount(pixelBufferCopy!) {
                let dest = CVPixelBufferGetBaseAddressOfPlane(pixelBufferCopy!, plane)
                let height = CVPixelBufferGetHeightOfPlane(pixelBufferCopy!, plane)
                let bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(pixelBufferCopy!, plane)
                let planceSize = height * bytesPerRow
                memcpy(dest, source, planceSize)
                source += planceSize
            }
            return pixelBufferCopy
        }
        self.width = width
        self.height = height
        if (textureId != -1) {
            registry?.textureFrameAvailable(textureId)
            return true
        } else {
            return false
        }
    }
    
}


