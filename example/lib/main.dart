import 'dart:math';
import 'dart:typed_data';

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

  @override
  void initState() {
    super.initState();
    _textureRgbaRendererPlugin.createTexture(key).then((textureId) {
      if (textureId != -1) {
        debugPrint("Texture register success, textureId=$textureId");
        _textureRgbaRendererPlugin.getTexturePtr(key).then((value) {
          debugPrint("texture ptr: $value");
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
      data = mockPicture(width, height);
      final res =
          await _textureRgbaRendererPlugin.onRgba(key, data!, height, width);
      if (!res) {
        debugPrint("WARN: render failed");
      }
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
                  "texture id: $textureId, texture memory address: $texturePtr"),
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
