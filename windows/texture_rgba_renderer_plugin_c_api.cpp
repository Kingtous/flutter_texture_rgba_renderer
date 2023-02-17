#include "include/texture_rgba_renderer/texture_rgba_renderer_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "texture_rgba_renderer_plugin.h"
#include "texture_rgba.h"

void TextureRgbaRendererPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  texture_rgba_renderer::TextureRgbaRendererPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

void FlutterRgbaRendererPluginOnRgba(void* texture_rgba, const uint8_t* buffer, int width, int height) {
    TextureRgba* rgba = static_cast<TextureRgba*>(texture_rgba);
    rgba->MarkVideoFrameAvailablePtr(buffer, width, height);
}
