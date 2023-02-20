import 'dart:ffi';
import 'dart:math';
import 'dart:typed_data';
import 'package:ffi/ffi.dart';

import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:texture_rgba_renderer/texture_rgba_renderer.dart';

typedef F1 = Void Function(
    Pointer<Void> ptr, Pointer<Uint8> buffer, Int32 width, Int32 height);
typedef F1Dart = void Function(
    Pointer<Void> ptr, Pointer<Uint8> buffer, int width, int height);

class Native {
  Native._();

  static get _instance => Native._()..init();
  static get instance => _instance;

  late F1Dart onRgba;

  init() {
    final lib = DynamicLibrary.process();
    onRgba = lib.lookupFunction<F1, F1Dart>("FlutterRgbaRendererPluginOnRgba");
  }
}

void main() {
  // Load the native instance.
  final instance = Native.instance;
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final _textureRgbaRendererPlugin = TextureRgbaRenderer();
  int textureId = -1;
  int height = 500;
  int width = 500;
  int cnt = 0;
  var key = 0;
  int texturePtr = 0;
  final random = Random();
  Uint8List? data;

  @override
  void initState() {
    super.initState();
    _textureRgbaRendererPlugin.createTexture(key).then((textureId) {
      if (textureId != -1) {
        debugPrint("Texture register success, textureId=$textureId");
        _textureRgbaRendererPlugin.getTexturePtr(key).then((value) {
          debugPrint("texture ptr: ${value.toRadixString(16)}");
          setState(() {
            texturePtr = value;
          });
        });
        setState(() {
          this.textureId = textureId;
        });
      } else {
        return;
      }
    });
  }

  void start() {
    debugPrint("start mockPic");
    Timer.periodic(const Duration(milliseconds: 30), (timer) async {
      // data = mockPicture(width, height);
      // Method.1: with MethodChannel
      // final res =
      //     await _textureRgbaRendererPlugin.onRgba(key, data!, height, width);
      // if (!res) {
      //   debugPrint("WARN: render failed");
      // }
      final dataPtr = mockPicturePtr(width, height);
      // Method.2: with native ffi
      Native.instance.onRgba(
          Pointer.fromAddress(texturePtr).cast<Void>(), dataPtr, width, height);
      malloc.free(dataPtr);
    });
  }

  @override
  void dispose() {
    if (key != -1) {
      _textureRgbaRendererPlugin.closeTexture(key);
    }
    super.dispose();
  }

  Uint8List mockPicture(int width, int height) {
    final pic = List.generate(width * height * 4, (index) {
      return random.nextInt(255);
    });
    return Uint8List.fromList(pic);
  }

  Pointer<Uint8> mockPicturePtr(int width, int height) {
    final pic = List.generate(width * height * 4, (index) {
      return random.nextInt(255);
    });
    final pic_addr = malloc.allocate(pic.length).cast<Uint8>();
    final list = pic_addr.asTypedList(pic.length);
    list.setRange(0, pic.length, pic);
    return pic_addr;
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Stack(
          children: [
            textureId == -1
                ? const Offstage()
                : Container(
                    alignment: Alignment.center,
                    child: Container(
                        decoration: const BoxDecoration(color: Colors.black),
                        width: width.toDouble(),
                        height: height.toDouble(),
                        child: Texture(textureId: textureId)),
                  ),
            Align(
              alignment: Alignment.topLeft,
              child: Text(
                  "texture id: $textureId, texture memory address: ${texturePtr.toRadixString(16)}"),
            ),
            Align(
              alignment: Alignment.bottomCenter,
              child: IconButton(
                icon: const Icon(Icons.play_arrow),
                onPressed: start,
              ),
            )
          ],
        ),
      ),
    );
  }
}
