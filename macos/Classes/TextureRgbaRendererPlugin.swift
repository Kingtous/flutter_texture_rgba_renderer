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
            texture_registry?.unregisterTexture(renderer[key]?.textureId ?? -1)
            result(true)
        } else {
            result(false)
        }
        
    case "onRgba":
        let args = call.arguments as! Dictionary<String, Any?>
        let key = args["key"] as! Int64
        let data = args["data"] as! Data
        let height = args["height"] as! Int
        let width = args["height"] as! Int
        let textureRgba = renderer[key]
        if (textureRgba == nil) {
            result(false)
        } else {
            result(textureRgba!.markFrameAvaliable(data: data, width: width, height: height))
        }
    default:
      result(FlutterMethodNotImplemented)
    }
  }
}
