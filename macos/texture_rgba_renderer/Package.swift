// swift-tools-version: 5.9

import PackageDescription

let package = Package(
    name: "texture_rgba_renderer",
    platforms: [
        .macOS("10.15")
    ],
    products: [
        .library(name: "texture-rgba-renderer", targets: ["texture_rgba_renderer"])
    ],
    dependencies: [
        // Flutter supplies this local package when building the host app.
        .package(name: "FlutterFramework", path: "../FlutterFramework")
    ],
    targets: [
        .target(
            name: "texture_rgba_renderer",
            dependencies: [
                .product(name: "FlutterFramework", package: "FlutterFramework")
            ]
        )
    ]
)
