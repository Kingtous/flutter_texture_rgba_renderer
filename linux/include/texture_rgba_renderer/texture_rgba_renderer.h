#ifndef TEXTURE_RGBA_RENDERER_H
#define TEXTURE_RGBA_RENDERER_H

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>

#include "texture_rgba_renderer_plugin.h"

struct _TextureRgbaClass {
    FlPixelBufferTextureClass parent_class;
};

struct TextureRgbaPrivate
{
  int64_t texture_id = 0;
  uint8_t* buffer = nullptr;
  int32_t video_width = 0;
  int32_t video_height = 0;
};

/// Constructor.
G_DECLARE_DERIVABLE_TYPE(TextureRgba, texture_rgba, TEXTURE_RGBA_RENDERER, TEXTURE_RGBA, FlPixelBufferTexture)
/// Add private data.
G_DEFINE_TYPE_WITH_CODE(TextureRgba, texture_rgba,
                        fl_pixel_buffer_texture_get_type(),
                        G_ADD_PRIVATE(TextureRgba))


static gboolean texture_rgba_copy_pixels(FlPixelBufferTexture* texture,
                                         const uint8_t** out_buffer,
                                         uint32_t* width, uint32_t* height,
                                         GError** error) {
  auto texture_rgba_private =
      (TextureRgbaPrivate*)texture_rgba_get_instance_private(
          TEXTURE_RGBA_RENDERER_TEXTURE_RGBA(texture));
  *out_buffer = texture_rgba_private->buffer;
  *width = texture_rgba_private->video_width;
  *height = texture_rgba_private->video_height;
  return TRUE;
}

static TextureRgba* texture_rgba_new() {
  return TEXTURE_RGBA_RENDERER_TEXTURE_RGBA(g_object_new(texture_rgba_get_type(), nullptr));
}

static void texture_rgba_class_init(TextureRgbaClass* klass) {
  FL_PIXEL_BUFFER_TEXTURE_CLASS(klass)->copy_pixels = texture_rgba_copy_pixels;
}

static void texture_rgba_init(TextureRgba* self) {}

#endif