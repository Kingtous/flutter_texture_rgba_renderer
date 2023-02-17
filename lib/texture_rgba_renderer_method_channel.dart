import 'dart:typed_data';

import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'texture_rgba_renderer_platform_interface.dart';

/// An implementation of [TextureRgbaRendererPlatform] that uses method channels.
class MethodChannelTextureRgbaRenderer extends TextureRgbaRendererPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('texture_rgba_renderer');

  @override
  Future<String?> getPlatformVersion() async {
    final version =
        await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<int> createTexture(int key) async {
    return await methodChannel
            .invokeMethod<int>('createTexture', {"key": key}) ??
        -1;
  }

  @override
  Future<bool> closeTexture(int key) async {
    return await methodChannel
            .invokeMethod<bool>('closeTexture', {"key": key}) ??
        false;
  }

  @override
  Future<bool> onRgba(int key, Uint8List data, int height, int width) async {
    assert(data.length == height * width * 4);
    return await methodChannel.invokeMethod<bool>('onRgba',
            {"data": data, "height": height, "width": width, "key": key}) ??
        false;
  }
}
