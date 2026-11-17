import 'dart:io';

import 'package:flutter_test/flutter_test.dart';
import 'package:integration_test/integration_test.dart';

import 'native_smoke.dart';

void main() {
  IntegrationTestWidgetsFlutterBinding.ensureInitialized();

  testWidgets('macOS native plugin lifecycle', (tester) async {
    await exerciseNativePlugin();
  }, skip: !Platform.isMacOS);
}
