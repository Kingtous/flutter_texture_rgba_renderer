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
  GHashTable *renderer_map;
};

G_DEFINE_TYPE(TextureRgbaRendererPlugin, texture_rgba_renderer_plugin, g_object_get_type())


static TextureRgba* texture_rgba_renderer_plugin_lookup_renderer(TextureRgbaRendererPlugin *self, gint64 key) {
  TextureRgba* val = (TextureRgba*)g_hash_table_lookup(self->renderer_map, GINT_TO_POINTER(key));
  return val;
}

static gboolean texture_rgba_renderer_plugin_add_renderer(TextureRgbaRendererPlugin *self, gint64 key, TextureRgba* rgba) {
  GValue* val = (GValue*)g_hash_table_lookup(self->renderer_map, GINT_TO_POINTER(key));
  if (val) {
    return FALSE;
  } else {
    return g_hash_table_insert(self->renderer_map, GINT_TO_POINTER(key), rgba);
  }
}

static gboolean texture_rgba_renderer_plugin_remove_renderer(TextureRgbaRendererPlugin *self, gint64 key) {
  return g_hash_table_remove(self->renderer_map, GINT_TO_POINTER(key));
}

// Called when a method call is received from Flutter.
static void texture_rgba_renderer_plugin_handle_method_call(
    TextureRgbaRendererPlugin *self,
    FlMethodCall *method_call)
{
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar *method = fl_method_call_get_name(method_call);

  if (strcmp(method, "createTexture") == 0)
  {
    auto args = fl_method_call_get_args(method_call);
    int64_t key = fl_value_get_int(fl_value_lookup_string(args, "key"));
    if (texture_rgba_renderer_plugin_lookup_renderer(self, key))
    {
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_int(-1)));
    }
    else
    {
      TextureRgba* texture_rgba = texture_rgba_new(self->texture_registrar);
      auto texture_id = reinterpret_cast<int64_t>(FL_TEXTURE(texture_rgba));
      if (texture_rgba_renderer_plugin_add_renderer(self, key, texture_rgba)) {
        // Register to the registrar.
        auto ret = fl_texture_registrar_register_texture(self->texture_registrar, FL_TEXTURE(texture_rgba));
        if (!ret) {
          fprintf(stderr, "error on register the texture");
          fflush(stderr);
        } else {
          fprintf(stderr, "flutter: register success.\n");
        }
        response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_int(texture_id)));
      } else {
        response = FL_METHOD_RESPONSE(fl_method_success_response_new(fl_value_new_int(-1)));
      }
    }
  }
  else if (strcmp(method, "closeTexture") == 0)
  {
    auto args = fl_method_call_get_args(method_call);
    int64_t key = fl_value_get_int(fl_value_lookup_string(args, "key"));
    auto* renderer = texture_rgba_renderer_plugin_lookup_renderer(self, key);
    if (renderer)
    {
      fl_texture_registrar_unregister_texture(self->texture_registrar, FL_TEXTURE(renderer));
      texture_rgba_renderer_plugin_remove_renderer(self, key);
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
    auto texture_rgba = texture_rgba_renderer_plugin_lookup_renderer(self, key);
    FlutterRgbaRendererPluginOnRgba((void*)texture_rgba, data, width, height);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(
        fl_value_new_bool(fl_texture_registrar_mark_texture_frame_available(self->texture_registrar, FL_TEXTURE(texture_rgba)))));
  }
  else if (strcmp(method, "getTexturePtr") == 0)
  {
    auto args = fl_method_call_get_args(method_call);
    auto key = fl_value_get_int(fl_value_lookup_string(args, "key"));
    auto renderer = texture_rgba_renderer_plugin_lookup_renderer(self, key);
    if (!renderer)
    {
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(
          fl_value_new_int(0)));
    }
    else
    {
      // Return an address.
      int64_t rgba = reinterpret_cast<int64_t>((void *)renderer);
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
  g_hash_table_unref(TEXTURE_RGBA_RENDERER_PLUGIN(object)->renderer_map);
  G_OBJECT_CLASS(texture_rgba_renderer_plugin_parent_class)->dispose(object);
}

static void texture_rgba_renderer_plugin_class_init(TextureRgbaRendererPluginClass *klass)
{
  G_OBJECT_CLASS(klass)->dispose = texture_rgba_renderer_plugin_dispose;
}

static void texture_rgba_renderer_plugin_init(TextureRgbaRendererPlugin *self)
{
  self->renderer_map = g_hash_table_new(g_direct_hash, g_direct_equal);
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
        if (!g_atomic_int_get(&priv->buffer_ready)) {
          g_atomic_int_set(&priv->buffer_ready, TRUE);
          fl_texture_registrar_mark_texture_frame_available(
            priv->texture_registrar,
            FL_TEXTURE(texture_rgba)
          );
        }
      }
      g_mutex_unlock(&priv->mutex);
   }
};
