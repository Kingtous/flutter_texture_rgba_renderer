#include "include/texture_rgba_renderer/texture_rgba_renderer_plugin.h"
#include "include/texture_rgba_renderer/texture_rgba_renderer.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#define TEXTURE_RGBA_RENDERER_PLUGIN(obj)                                     \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), texture_rgba_renderer_plugin_get_type(), \
                              TextureRgbaRendererPlugin))

#include <cstring>
#include <unordered_map>

void FlutterRgbaRendererPluginOnRgba(void *texture_rgba_ptr, const uint8_t *buffer, int width, int height);

struct _TextureRgbaRendererPlugin
{
  GObject parent_instance;
  FlTextureRegistrar *texture_registrar;
  std::unordered_map<int64_t, TextureRgba *> *g_renderer_map;
};

G_DEFINE_TYPE(TextureRgbaRendererPlugin, texture_rgba_renderer_plugin, g_object_get_type())

// Called when a method call is received from Flutter.
static void texture_rgba_renderer_plugin_handle_method_call(
    TextureRgbaRendererPlugin *self,
    FlMethodCall *method_call)
{
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar *method = fl_method_call_get_name(method_call);

  if (strcmp(method, "getPlatformVersion") == 0)
  {
    struct utsname uname_data = {};
    uname(&uname_data);
    g_autofree gchar *version = g_strdup_printf("Linux %s", uname_data.version);
    g_autoptr(FlValue) result = fl_value_new_string(version);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  }
  else if (strcmp(method, "createTexture") == 0)
  {
    auto args = fl_method_call_get_args(method_call);
    int64_t key = fl_value_get_int(fl_value_lookup_string(args, "key"));
    if (self->g_renderer_map->find(key) != self->g_renderer_map->end())
    {
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_int(-1)));
    }
    else
    {
      g_autoptr(TextureRgba) texture_rgba = texture_rgba_new(self->texture_registrar);
      auto texture_id = reinterpret_cast<int64_t>(FL_TEXTURE(texture_rgba));
      FL_TEXTURE_GL_GET_CLASS(texture_rgba)->populate =
          texture_rgba_populate;
      self->g_renderer_map->insert(std::make_pair(key, texture_rgba));
      // Register to the registrar.
      fl_texture_registrar_register_texture(self->texture_registrar, FL_TEXTURE(texture_rgba));
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_int(texture_id)));
    }
  }
  else if (strcmp(method, "closeTexture") == 0)
  {
    auto args = fl_method_call_get_args(method_call);
    int64_t key = fl_value_get_int(fl_value_lookup_string(args, "key"));
    if (self->g_renderer_map->find(key) != self->g_renderer_map->end())
    {
      fl_texture_registrar_unregister_texture(self->texture_registrar, FL_TEXTURE((*self->g_renderer_map)[key]));
      self->g_renderer_map->erase(key);
    }
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_bool(true)));
  }
  else if (strcmp(method, "onRgba") == 0)
  {
    auto args = fl_method_call_get_args(method_call);
    auto key = fl_value_get_int(fl_value_lookup_string(args, "key"));
    auto data = fl_value_get_uint8_list(fl_value_lookup_string(args, "data"));
    // auto data_length = fl_value_get_length(fl_value_lookup_string(args, "data"));
    auto width = fl_value_get_int(fl_value_lookup_string(args, "width"));
    auto height = fl_value_get_int(fl_value_lookup_string(args, "height"));
    #ifdef _DEBUG
    g_assert(data_length == width * height * 4);
    #endif
    auto texture_rgba = (*self->g_renderer_map)[key];
    FlutterRgbaRendererPluginOnRgba((void*)texture_rgba, data, width, height);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(
        fl_value_new_bool(fl_texture_registrar_mark_texture_frame_available(self->texture_registrar, FL_TEXTURE(texture_rgba)))));
  }
  else if (strcmp(method, "getTexturePtr") == 0)
  {
    auto args = fl_method_call_get_args(method_call);
    auto key = fl_value_get_int(fl_value_lookup_string(args, "key"));
    if (self->g_renderer_map->find(key) == self->g_renderer_map->end())
    {
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(
          fl_value_new_int(0)));
    }
    else
    {
      // Return an address.
      size_t rgba = reinterpret_cast<size_t>((void *)(*self->g_renderer_map)[key]);
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(
          fl_value_new_int(rgba)));
    }
  }
  else
  {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, nullptr);
}

static void texture_rgba_renderer_plugin_dispose(GObject *object)
{
  TEXTURE_RGBA_RENDERER_PLUGIN(object)->g_renderer_map->clear();
  G_OBJECT_CLASS(texture_rgba_renderer_plugin_parent_class)->dispose(object);
}

static void texture_rgba_renderer_plugin_class_init(TextureRgbaRendererPluginClass *klass)
{
  G_OBJECT_CLASS(klass)->dispose = texture_rgba_renderer_plugin_dispose;
}

static void texture_rgba_renderer_plugin_init(TextureRgbaRendererPlugin *self)
{
  self->g_renderer_map = new std::unordered_map<int64_t, TextureRgba *>();
}

static void method_call_cb(FlMethodChannel *channel, FlMethodCall *method_call,
                           gpointer user_data)
{
  TextureRgbaRendererPlugin *plugin = TEXTURE_RGBA_RENDERER_PLUGIN(user_data);
  texture_rgba_renderer_plugin_handle_method_call(plugin, method_call);
}

void texture_rgba_renderer_plugin_register_with_registrar(FlPluginRegistrar *registrar)
{
  TextureRgbaRendererPlugin *plugin = TEXTURE_RGBA_RENDERER_PLUGIN(
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


extern "C" {
   void FlutterRgbaRendererPluginOnRgba(void *texture_rgba_ptr, const uint8_t *buffer, int width, int height) {
      TextureRgba* texture_rgba = TEXTURE_RGBA_RENDERER_TEXTURE_RGBA(texture_rgba_ptr);
      auto priv = (TextureRgbaPrivate *)texture_rgba_get_instance_private(texture_rgba);
      g_mutex_lock(&priv->mutex);
      // private has registered a texture_id,
      if (priv->texture_id != 0) {
        // copy data to the texture.
        auto buffer_length = 4 * width * height;
        auto copied_data = new uint8_t[buffer_length];
        memcpy(copied_data, buffer, buffer_length); 
        // It's safe to working on a non reading index
        auto current_working_index = priv->current_reading_index ^ 1;
        // Dropped frame, let's free this.
        if (priv->buffer[current_working_index] != nullptr) {
          delete[] priv->buffer[current_working_index];
        }
        priv->buffer[current_working_index] = copied_data;
        priv->video_height[current_working_index] = height;
        priv->video_width[current_working_index] = width;
        if (!priv->buffer_ready) {
          priv->buffer_ready = true;
          fl_texture_registrar_mark_texture_frame_available(
            priv->texture_registrar,
            FL_TEXTURE(texture_rgba)
          );
        }
      }
      g_mutex_unlock(&priv->mutex);
   }
};
