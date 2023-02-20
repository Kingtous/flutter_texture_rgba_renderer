import Cocoa
import FlutterMacOS

public class TextureRgbaRendererPlugin: NSObject, FlutterPlugin {
    
    private var renderer: Dictionary<Int64, TextRgba> = [:]
    private var texture_registry: FlutterTextureRegistry?
    
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "texture_rgba_renderer", binaryMessenger: registrar.messenger)
    let instance = TextureRgbaRendererPlugin()
    instance.setTextureRegistry(registry: registrar.textures)
    registrar.addMethodCallDelegate(instance, channel: channel)
  }
    
    public func setTextureRegistry(registry: FlutterTextureRegistry) {
        texture_registry = registry
    }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "getPlatformVersion":
      result("macOS " + ProcessInfo.processInfo.operatingSystemVersionString)
    case "createTexture":
        let args = call.arguments as! Dictionary<String, Any?>
        let key = args["key"] as! Int64
        if (renderer[key] != nil) {
            result(-1)
        } else {
            let textRgba = TextRgba.new(registry: texture_registry)
            renderer[key] = textRgba
            result(textRgba.textureId)
        }
    case "closeTexture":
        let args = call.arguments as! Dictionary<String, Any?>
        let key = args["key"] as! Int64
        if (renderer[key] != nil) {
            if (-1 != renderer[key]?.textureId) {
                texture_registry?.unregisterTexture(renderer[key]?.textureId ?? -1)
            }
            result(true)
        } else {
            result(false)
        }
        
    case "onRgba":
        let args = call.arguments as! Dictionary<String, Any?>
        let key = args["key"] as! Int64
        let data = args["data"] as! FlutterStandardTypedData
        let height = args["height"] as! Int
        let width = args["height"] as! Int
        let textureRgba = renderer[key]
        if (textureRgba == nil) {
            result(false)
        } else {
            result(textureRgba!.markFrameAvaliable(data: data.data, width: width, height: height))
        }
    case "getTexturePtr":
        let args = call.arguments as! Dictionary<String, Any?>
        let key = args["key"] as! Int64
        let textureRgba = renderer[key]
        if (textureRgba == nil) {
            result(0)
        } else {
            let ptr = withUnsafePointer(to: textureRgba!) { p in
                return UnsafeRawPointer(p);
            };
            let intAddr = UInt(bitPattern: ptr)
            result(intAddr)
        }
    default:
      result(FlutterMethodNotImplemented)
    }
  }
}

