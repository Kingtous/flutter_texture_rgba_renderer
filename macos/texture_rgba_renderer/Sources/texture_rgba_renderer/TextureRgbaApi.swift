import Foundation

// Preserve the native API's C symbol and 32-bit C int parameters for FFI callers.
// Implementing the wrapper in Swift keeps this a single-language SwiftPM target.
@_cdecl("FlutterRgbaRendererPluginOnRgba")
public func FlutterRgbaRendererPluginOnRgba(
    _ textureRgbaPtr: UnsafeMutableRawPointer,
    _ buffer: UnsafePointer<UInt8>,
    _ len: Int32,
    _ width: Int32,
    _ height: Int32,
    _ strideAlign: Int32
) {
    let textureRgba = Unmanaged<TextRgba>.fromOpaque(textureRgbaPtr).takeUnretainedValue()
    _ = textureRgba.markFrameAvaliableRaw(
        buffer: buffer,
        len: Int(len),
        width: Int(width),
        height: Int(height),
        stride_align: Int(strideAlign)
    )
}
