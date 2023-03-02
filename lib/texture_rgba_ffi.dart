import 'dart:ffi';

typedef F1 = Void Function(Pointer<Void> ptr, Pointer<Uint8> buffer,
    Int32 width, Int32 height, Int stride_align);
typedef F1Dart = void Function(Pointer<Void> ptr, Pointer<Uint8> buffer,
    int width, int height, int stride_align);

class Native {
  Native._();

  static Native get _instance => Native._()..init();
  static Native get instance => _instance;

  late F1Dart onRgba;

  init() {
    final lib = DynamicLibrary.process();
    onRgba = lib.lookupFunction<F1, F1Dart>("FlutterRgbaRendererPluginOnRgba");
  }
}
