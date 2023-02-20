import 'dart:ffi';
import 'dart:math';
import 'dart:typed_data';
import 'package:ffi/ffi.dart';

import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:texture_rgba_renderer/texture_rgba_renderer.dart';

void main() {
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
  Timer? _timer;

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
    _timer?.cancel();
    _timer = Timer.periodic(const Duration(milliseconds: 30), (timer) async {
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
    _timer?.cancel();
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
    final picAddr = malloc.allocate(pic.length).cast<Uint8>();
    final list = picAddr.asTypedList(pic.length);
    list.setRange(0, pic.length, pic);
    return picAddr;
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        body: Column(
          crossAxisAlignment: CrossAxisAlignment.center,
          mainAxisAlignment: MainAxisAlignment.center,
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
            Text(
                "texture id: $textureId, texture memory address: ${texturePtr.toRadixString(16)}"),
            IconButton(
              icon: const Icon(Icons.play_arrow),
              onPressed: start,
            ),
          ],
        ),
      ),
    );
  }
}
