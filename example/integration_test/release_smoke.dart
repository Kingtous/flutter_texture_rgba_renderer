import 'dart:io';

import 'package:flutter/widgets.dart';

import 'native_smoke.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();
  try {
    await exerciseNativePlugin();
    stdout.writeln('Native release smoke test passed.');
    exit(0);
  } catch (error, stack) {
    stderr.writeln('$error\n$stack');
    exit(1);
  }
}
