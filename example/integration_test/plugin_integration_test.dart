import 'dart:ffi';
import 'dart:io';
import 'dart:typed_data';

import 'package:ffi/ffi.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:integration_test/integration_test.dart';
import 'package:texture_rgba_renderer/texture_rgba_renderer.dart';

void main() {
  IntegrationTestWidgetsFlutterBinding.ensureInitialized();

  testWidgets('macOS method channel and exported native API', (tester) async {
    final plugin = TextureRgbaRenderer();
    const key = 314;
    // One 16-pixel BGRA row matches the macOS renderer's 64-byte alignment.
    const width = 16;
    const height = 1;
    const strideAlign = 64;
    final bytes = Uint8List(64)..fillRange(0, 64, 0x7f);
    final textureId = await plugin.createTexture(key);
    expect(textureId, greaterThanOrEqualTo(0));
    try {
      expect(
        await plugin.onRgba(key, bytes, height, width, strideAlign),
        isTrue,
      );
      final texturePtr = await plugin.getTexturePtr(key);
      expect(texturePtr, isNonZero);
      final buffer = calloc<Uint8>(bytes.length);
      try {
        buffer.asTypedList(bytes.length).setAll(0, bytes);
        // Resolves the C symbol from the running app, then invokes its ABI.
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
      expect(await plugin.closeTexture(key), isTrue);
    }
  }, skip: !Platform.isMacOS);
}
