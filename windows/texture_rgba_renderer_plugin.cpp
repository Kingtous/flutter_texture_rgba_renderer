#include "texture_rgba_renderer_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

namespace texture_rgba_renderer {

// static
void TextureRgbaRendererPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "texture_rgba_renderer",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<TextureRgbaRendererPlugin>();
  plugin->texture_registrar = registrar->texture_registrar();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

TextureRgbaRendererPlugin::TextureRgbaRendererPlugin() {}

TextureRgbaRendererPlugin::~TextureRgbaRendererPlugin() {}

void TextureRgbaRendererPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("createTexture") == 0) {
     auto args = std::get<flutter::EncodableMap*>(method_call.arguments());
     arg
  } else {
    result->NotImplemented();
  }
}

}  // namespace texture_rgba_renderer
