# Texture Rgba Renderer 

A texture helper which brings a high level api to handle RGBA data. No frame copy required, and with full hardware acceleration.

![Pub Version](https://img.shields.io/pub/v/texture_rgba_renderer)
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2FKingtous%2Fflutter_texture_rgba_renderer.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2FKingtous%2Fflutter_texture_rgba_renderer?ref=badge_shield)

## Platform Support

The plugin aims to provide opengl impl for the texture, which bring a better performance instead of `FFI` + `CustomPainter`.

- [x] Linux
- [x] Windows (No gpu acceleration yet due to the limitation of Flutter.)

```c++
// The available texture variants.
// Only PixelBufferTexture is currently implemented.
// Other variants are expected to be added in the future.
typedef std::variant<PixelBufferTexture, GpuSurfaceTexture> TextureVariant;
```

- [x] MacOS

Note: due to the limitation of supported formats for `CVPixelBuffer`, please use the `BGRA` format instead.

## Getting Started

### Create a texture by calling `createTexture`.

```dart
_textureRgbaRendererPlugin.createTexture(key).then((textureId) {
      if (textureId != -1) {
        debugPrint("Texture register success, textureId=$textureId");
        setState(() {
          this.textureId = textureId;
        });
      } else {
        return;
      }
    });
```

### Transfer rgba data by calling `onRgba`.

```dart
data = mockPicture(width, height);
final res = await _textureRgbaRendererPlugin.onRgba(key, data!, height, width);
if (!res) {
    debugPrint("WARN: render failed");
}
```

### Close a texture by calling `closeTexture`.
```dart
if (key != -1) {
    _textureRgbaRendererPlugin.closeTexture(key);
}
```

Check the example for details.

## API

```dart
class TextureRgbaRenderer {

  /// Create a texture with unique identifier [key].
  /// 
  /// @return a texture id which can be used with
  /// ```dart
  /// Texture(textureId: textureId)
  /// ```
  Future<int> createTexture(int key) {
    return TextureRgbaRendererPlatform.instance.createTexture(key); 
  }

  /// Close a texture with unique identifier [key].
  /// 
  /// @return a boolean to indicate whether the operation is sucessfully executed.
  Future<bool> closeTexture(int key) {
    return TextureRgbaRendererPlatform.instance.closeTexture(key); 
  }

  /// Provide the rgba data to the texture.
  Future<bool> onRgba(int key, Uint8List data, int height, int width) {
     return TextureRgbaRendererPlatform.instance.onRgba(key, data, height, width); 
  }
}
```

## Thanks

- [Flutter](https://github.com/flutter/flutter)
- [Dart VLC](https://github.com/alexmercerind/dart_vlc) 

## License
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2FKingtous%2Fflutter_texture_rgba_renderer.svg?type=large)](https://app.fossa.com/projects/git%2Bgithub.com%2FKingtous%2Fflutter_texture_rgba_renderer?ref=badge_large)