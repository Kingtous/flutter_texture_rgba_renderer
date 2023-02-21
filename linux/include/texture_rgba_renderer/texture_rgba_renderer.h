#ifndef TEXTURE_RGBA_RENDERER_H
#define TEXTURE_RGBA_RENDERER_H

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <epoxy/gl.h>

#include "texture_rgba_renderer_plugin.h"

#include <vector>

struct _TextureRgbaClass
{
  FlTextureGLClass parent_class;
};

struct TextureRgbaPrivate
{
  FlTextureRegistrar* texture_registrar = nullptr;
  GLuint texture_id = 0;
  const uint8_t *buffer[2] = {nullptr, nullptr};
  size_t current_reading_index = 0;
  bool buffer_ready = false;
  int32_t video_width[2] = {0, 0};
  int32_t video_height[2] = {0, 0};
  GMutex mutex;
};

/// Constructor.
G_DECLARE_DERIVABLE_TYPE(TextureRgba, texture_rgba, TEXTURE_RGBA_RENDERER, TEXTURE_RGBA, FlTextureGL)
/// Add private data.
G_DEFINE_TYPE_WITH_CODE(TextureRgba, texture_rgba,
                        fl_texture_gl_get_type(),
                        G_ADD_PRIVATE(TextureRgba))

static gboolean texture_rgba_populate(FlTextureGL *texture,
                                      uint32_t *target,
                                      uint32_t *name,
                                      uint32_t *width,
                                      uint32_t *height,
                                      GError **error)
{
  TextureRgbaPrivate *self = (TextureRgbaPrivate *)
      texture_rgba_get_instance_private(TEXTURE_RGBA_RENDERER_TEXTURE_RGBA(texture));
  
  g_mutex_lock(&self->mutex);
  if (self->buffer_ready) {
    self->current_reading_index ^= 1;
  }
  auto index = self->current_reading_index;

  if (self->texture_id == 0)
  {
    glGenTextures(1, &self->texture_id);
    glBindTexture(GL_TEXTURE_2D, self->texture_id);
    // further configuration here.
  }
  else
  {
    glBindTexture(GL_TEXTURE_2D, self->texture_id);
  }
  // For example, we render pixel buffer here.
  // Note that Flutter only accepts textures in GL_RGBA8 format.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, self->video_width[index], self->video_height[index], 0, GL_BGRA,
               GL_UNSIGNED_BYTE, self->buffer[index]);
  // Now we can release the self->buffer.
  if (self->buffer[index] != nullptr) {
    delete[] self->buffer[index];
    self->buffer[index] = nullptr;
  }
  self->buffer_ready = false;
  *target = GL_TEXTURE_2D;
  *name = self->texture_id;
  *width = self->video_width[index];
  *height = self->video_height[index];
  g_mutex_unlock(&self->mutex);
  return TRUE;
}

static TextureRgba *texture_rgba_new(FlTextureRegistrar* registrar)
{
  auto texture = TEXTURE_RGBA_RENDERER_TEXTURE_RGBA(g_object_new(texture_rgba_get_type(), nullptr));
  ((TextureRgbaPrivate*)texture_rgba_get_instance_private(texture))->texture_registrar = registrar;
  g_mutex_init(&((TextureRgbaPrivate*)texture_rgba_get_instance_private(texture))->mutex);
  return texture;
}

static void texture_rgba_class_init(TextureRgbaClass *klass)
{
  FL_TEXTURE_GL_GET_CLASS(klass)->populate = texture_rgba_populate;
}

static void texture_rgba_init(TextureRgba* self) {}

#endif