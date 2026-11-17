import 'dart:ffi';
import 'dart:typed_data';

import 'package:ffi/ffi.dart';
import 'package:texture_rgba_renderer/texture_rgba_renderer.dart';

Future<void> exerciseNativePlugin() async {
  final plugin = TextureRgbaRenderer();
  const key = 314;
  // One 16-pixel BGRA row matches the macOS renderer's 64-byte alignment.
  const width = 16;
  const height = 1;
  const strideAlign = 64;
  final bytes = Uint8List(64)..fillRange(0, 64, 0x7f);
  final textureId = await plugin.createTexture(key);
  if (textureId < 0) throw StateError('Texture creation failed.');
  try {
    if (!await plugin.onRgba(key, bytes, height, width, strideAlign)) {
      throw StateError('Method-channel upload failed.');
    }
    final texturePtr = await plugin.getTexturePtr(key);
    if (texturePtr == 0) throw StateError('Texture pointer is null.');
    final buffer = calloc<Uint8>(bytes.length);
    try {
      buffer.asTypedList(bytes.length).setAll(0, bytes);
      // Resolve the C symbol from the running app, then invoke its ABI.
      Native.instance.onRgba(
        Pointer<Void>.fromAddress(texturePtr),
        buffer,
        bytes.length,
        width,
        height,
        strideAlign,
      );
    } finally {
      calloc.free(buffer);
    }
  } finally {
    if (!await plugin.closeTexture(key)) {
      throw StateError('Texture cleanup failed.');
    }
  }
}
