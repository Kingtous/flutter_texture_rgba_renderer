# macOS dependency manager validation

Use macOS with Xcode, Flutter 3.41 or later (Dart 3.11 or later), and CocoaPods.
The plugin requires macOS 10.15 or later. Follow
[Flutter's migration guide](https://docs.flutter.dev/packages-and-plugins/swift-package-manager/for-plugin-authors).
Run the commands below from the repository root.

## Common checks

```sh
flutter pub get
flutter analyze
flutter test
flutter pub publish --dry-run
cd example
flutter pub get
cd ..
```

## CocoaPods

```sh
flutter config --no-enable-swift-package-manager
cd example
flutter clean
flutter pub get
flutter build macos --debug
flutter run -d macos
cd ..
pod lib lint macos/texture_rgba_renderer.podspec --configuration=Debug --skip-tests --use-modular-headers --use-libraries
pod lib lint macos/texture_rgba_renderer.podspec --configuration=Debug --skip-tests --use-modular-headers
```

## Swift Package Manager

```sh
flutter config --enable-swift-package-manager
cd example
flutter clean
flutter pub get
flutter build macos --debug
flutter run -d macos
flutter build macos --release
cd ..
```

Flutter supplies the sibling `FlutterFramework` package during app integration.
Standalone `swift build` in the plugin directory cannot validate the integration
without that generated package and the macOS Flutter engine.

Check the example's feature manually in both dependency modes. Restore your
preferred Flutter configuration afterward. Do not commit generated build files,
local package paths, or incidental example migration changes.

For the renderer, exercise both the method channel and native API in the example.
After each debug build, run the integration test from `example`:

```sh
flutter test integration_test/plugin_integration_test.dart -d macos
```

The test creates a texture, uploads through the method channel, looks up the
exported C symbol through `DynamicLibrary.process()`, invokes its ABI, and closes
the texture. The standalone release smoke app checks that release linking preserves the FFI entry point.

## GitHub Actions

The `macOS plugin CI` workflow runs on macOS for Flutter 3.41.0 and 3.50.0,
using both Swift Package Manager and CocoaPods. Every matrix job checks package
manifest syntax, runs Dart analysis and unit tests, builds the example in debug
and release, runs the debug native integration test, and executes a standalone release smoke app. Analyzer
errors fail CI; existing informational lints and warnings do not block it.
The workflow also verifies that the selected dependency manager includes this
plugin, so a successful fallback build cannot masquerade as SwiftPM coverage.

Flutter's desktop test/driver commands cannot run in release mode because they
require the VM service. The CI workflow instead builds and launches
`integration_test/release_smoke.dart`, which shares native checks with the debug
integration test and exits with a nonzero status on failure. Run it locally with:

```sh
cd example
flutter build macos --release --target=integration_test/release_smoke.dart
python3 -c "import subprocess; subprocess.run(['build/macos/Build/Products/Release/texture_rgba_renderer_example.app/Contents/MacOS/texture_rgba_renderer_example'], check=True, timeout=90)"
```
