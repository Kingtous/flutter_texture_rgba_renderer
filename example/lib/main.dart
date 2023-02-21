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
  int time = 0;
  int method = 0;

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

  void start(int methodId) {
    debugPrint("start mockPic");
    method = methodId;
    _timer?.cancel();
    // 60 fps
    _timer =
        Timer.periodic(const Duration(milliseconds: 1000 ~/ 60), (timer) async {
      if (methodId == 0) {
        // Method.1: with MethodChannel
        data = mockPicture(width, height);
        final t1 = DateTime.now().microsecondsSinceEpoch;
        final res =
            await _textureRgbaRendererPlugin.onRgba(key, data!, height, width);
        final t2 = DateTime.now().microsecondsSinceEpoch;
        setState(() {
          time = t2 - t1;
        });
        if (!res) {
          debugPrint("WARN: render failed");
        }
      } else {
        final dataPtr = mockPicturePtr(width, height);
        // Method.2: with native ffi
        final t1 = DateTime.now().microsecondsSinceEpoch;
        Native.instance.onRgba(Pointer.fromAddress(texturePtr).cast<Void>(),
            dataPtr, width, height);
        final t2 = DateTime.now().microsecondsSinceEpoch;
        setState(() {
          time = t2 - t1;
        });
        malloc.free(dataPtr);
      }
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
            Expanded(child: textureId == -1
                ? const Offstage()
                : Padding(
                  padding: const EdgeInsets.all(8.0),
                  child: Container(
                      alignment: Alignment.center,
                      decoration: const BoxDecoration(color: Colors.blue),
                      // decoration: const BoxDecoration(color: Colors.black),
                      // width: width.toDouble(),
                      // height: height.toDouble(),
                      child: Texture(textureId: textureId)),
                ),),
            Text(
                "texture id: $textureId, texture memory address: ${texturePtr.toRadixString(16)}"),
            TextButton.icon(
              label: const Text("play with texture (method channel API)"),
              icon: const Icon(Icons.play_arrow),
              onPressed: () => start(0),
            ),
            TextButton.icon(
              label: const Text("play with texture (native API, faster)"),
              icon: const Icon(Icons.play_arrow),
              onPressed: () => start(1),
            ),
            Text(
                "Current mode: ${method == 0 ? 'Method Channel API' : 'Native API'}"),
            time != 0
                ? Text("FPS: ${1000000 ~/ time} fps")
                : const Offstage()
          ],
        ),
      ),
    );
  }
}
