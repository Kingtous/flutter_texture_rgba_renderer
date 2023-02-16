import 'dart:typed_data';

import 'package:flutter_test/flutter_test.dart';
import 'package:texture_rgba_renderer/texture_rgba_renderer_platform_interface.dart';
import 'package:texture_rgba_renderer/texture_rgba_renderer_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockTextureRgbaRendererPlatform
    with MockPlatformInterfaceMixin
    implements TextureRgbaRendererPlatform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');

  @override
  Future<bool> closeTexture(int key) => Future.value(true);

  @override
  Future<int> createTexture(int key) => Future.value(1);

  @override
  Future<bool> onRgba(int key, Uint8List data, int height, int width) =>
      Future.value(true);
}

void main() {
  final TextureRgbaRendererPlatform initialPlatform =
      TextureRgbaRendererPlatform.instance;

  test('$MethodChannelTextureRgbaRenderer is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelTextureRgbaRenderer>());
  });
}
