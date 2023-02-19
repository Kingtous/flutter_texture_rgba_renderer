#ifndef TEXTURE_RGBA_RENDERER_H
#define TEXTURE_RGBA_RENDERER_H

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <epoxy/gl.h>

#include "texture_rgba_renderer_plugin.h"

struct _TextureRgbaClass
{
  FlTextureGLClass parent_class;
};

struct TextureRgbaPrivate
{
  FlTextureRegistrar* texture_registrar = nullptr;
  GLuint texture_id = 0;
  const uint8_t *buffer = nullptr;
  size_t buffer_length = 0;
  int32_t video_width = 0;
  int32_t video_height = 0;
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, self->video_width, self->video_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, self->buffer);
  delete[] self->buffer;
  self->buffer = nullptr;
  g_mutex_unlock(&self->mutex);
  *target = GL_TEXTURE_2D;
  *name = self->texture_id;
  *width = self->video_width;
  *height = self->video_height;

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