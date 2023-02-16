# Texture Rgba Renderer 

A texture helper which brings a high level api to handle RGBA data.

![Pub Version](https://img.shields.io/pub/v/flutter_texture_rgba_renderer)
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2FKingtous%2Fflutter_texture_rgba_renderer.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2FKingtous%2Fflutter_texture_rgba_renderer?ref=badge_shield)

## Platform Support

- [x] Linux
- [ ] Windows (not implemented yet.)
- [ ] MacOS (not implemented yet.)

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