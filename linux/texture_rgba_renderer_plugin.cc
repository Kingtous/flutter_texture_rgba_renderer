#include "include/texture_rgba_renderer/texture_rgba_renderer_plugin.h"
#include "include/texture_rgba_renderer/texture_rgba_renderer.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#define TEXTURE_RGBA_RENDERER_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), texture_rgba_renderer_plugin_get_type(), \
                              TextureRgbaRendererPlugin))

#include <cstring>
#include <unordered_map>

struct _TextureRgbaRendererPlugin {
  GObject parent_instance;
  FlTextureRegistrar* texture_registrar;
};

std::unordered_map<int64_t, TextureRgba*> g_renderer_map;

G_DEFINE_TYPE(TextureRgbaRendererPlugin, texture_rgba_renderer_plugin, g_object_get_type())

// Called when a method call is received from Flutter.
static void texture_rgba_renderer_plugin_handle_method_call(
    TextureRgbaRendererPlugin* self,
    FlMethodCall* method_call) {
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar* method = fl_method_call_get_name(method_call);

  if (strcmp(method, "getPlatformVersion") == 0) {
    struct utsname uname_data = {};
    uname(&uname_data);
    g_autofree gchar *version = g_strdup_printf("Linux %s", uname_data.version);
    g_autoptr(FlValue) result = fl_value_new_string(version);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  } else if (strcmp(method, "createTexture") == 0) {
    auto args = fl_method_call_get_args(method_call);
    int64_t key = fl_value_get_int(fl_value_lookup_string(args, "key"));
    if (g_renderer_map.find(key) != g_renderer_map.end()) {
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_int(-1)));
    } else {
      auto texture_rgba = texture_rgba_new();
      auto texture_rgba_private = (TextureRgbaPrivate*) texture_rgba_get_instance_private(texture_rgba);
      texture_rgba_private->texture_id = reinterpret_cast<int64_t>(FL_TEXTURE(texture_rgba));
      FL_PIXEL_BUFFER_TEXTURE_GET_CLASS(texture_rgba)->copy_pixels =
          texture_rgba_copy_pixels;
      g_renderer_map[key] = texture_rgba;
      // register to 
      fl_texture_registrar_register_texture(self->texture_registrar, FL_TEXTURE(texture_rgba));
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_int(texture_rgba_private->texture_id)));
    }
  } else if (strcmp(method, "closeTexture") == 0) {
    auto args = fl_method_call_get_args(method_call);
    int64_t key = fl_value_get_int(fl_value_lookup_string(args, "key"));
    if (g_renderer_map.find(key) != g_renderer_map.end()){
      fl_texture_registrar_unregister_texture(self->texture_registrar, FL_TEXTURE(g_renderer_map[key]));
    }
    g_renderer_map.erase(key);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_bool(true)));
  } else if (strcmp(method, "onRgba") == 0) {
    auto args = fl_method_call_get_args(method_call);
    auto key = fl_value_get_int(fl_value_lookup_string(args, "key"));
    auto data = fl_value_get_uint8_list(fl_value_lookup_string(args, "data"));
    auto data_length = fl_value_get_length(fl_value_lookup_string(args, "data"));
    auto buffer = new uint8_t[data_length];
    memcpy(buffer, data, data_length);
    auto width = fl_value_get_int(fl_value_lookup_string(args, "width"));
    auto height = fl_value_get_int(fl_value_lookup_string(args, "height"));
    auto texture_rgba = g_renderer_map[key];
    auto texture_rgba_private = (TextureRgbaPrivate*) texture_rgba_get_instance_private(g_renderer_map[key]);
    texture_rgba_private->buffer = buffer;
    texture_rgba_private->video_height = height;
    texture_rgba_private->video_width = width;
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(
      fl_value_new_bool(fl_texture_registrar_mark_texture_frame_available(self->texture_registrar, FL_TEXTURE(texture_rgba)))
    ));
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, nullptr);
}

static void texture_rgba_renderer_plugin_dispose(GObject* object) {
  G_OBJECT_CLASS(texture_rgba_renderer_plugin_parent_class)->dispose(object);
}

static void texture_rgba_renderer_plugin_class_init(TextureRgbaRendererPluginClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = texture_rgba_renderer_plugin_dispose;
}

static void texture_rgba_renderer_plugin_init(TextureRgbaRendererPlugin* self) {}

static void method_call_cb(FlMethodChannel* channel, FlMethodCall* method_call,
                           gpointer user_data) {
  TextureRgbaRendererPlugin* plugin = TEXTURE_RGBA_RENDERER_PLUGIN(user_data);
  texture_rgba_renderer_plugin_handle_method_call(plugin, method_call);
}

void texture_rgba_renderer_plugin_register_with_registrar(FlPluginRegistrar* registrar) {
  TextureRgbaRendererPlugin* plugin = TEXTURE_RGBA_RENDERER_PLUGIN(
      g_object_new(texture_rgba_renderer_plugin_get_type(), nullptr));

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "texture_rgba_renderer",
                            FL_METHOD_CODEC(codec));
  plugin->texture_registrar = fl_plugin_registrar_get_texture_registrar(registrar);
  fl_method_channel_set_method_call_handler(channel, method_call_cb,
                                            g_object_ref(plugin),
                                            g_object_unref);

  g_object_unref(plugin);
}
