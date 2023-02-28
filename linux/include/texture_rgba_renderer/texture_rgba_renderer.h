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
  const uint8_t *buffer = nullptr;
  gboolean buffer_ready = FALSE;
  gboolean terminated = FALSE;
  gint32 video_width = 0;
  gint32 video_height = 0;
  GMutex mutex;
};

/// Constructor.
G_DECLARE_DERIVABLE_TYPE(TextureRgba, texture_rgba, TEXTURE_RGBA_RENDERER, TEXTURE_RGBA, FlTextureGL)
/// Add private data.
G_DEFINE_TYPE_WITH_CODE(TextureRgba, texture_rgba,
                        fl_texture_gl_get_type(),
                        G_ADD_PRIVATE(TextureRgba))

static void texture_rgba_terminate(TextureRgba* texture) {
  TextureRgbaPrivate *self = (TextureRgbaPrivate *)
      texture_rgba_get_instance_private(TEXTURE_RGBA_RENDERER_TEXTURE_RGBA(texture));
  g_mutex_lock(&self->mutex);
  g_atomic_int_set(&self->terminated, TRUE);
  if (g_atomic_pointer_get(&self->buffer)) {
    delete[] g_atomic_pointer_get(&self->buffer);
    g_atomic_pointer_set(&self->buffer, nullptr);
  }
  g_mutex_unlock(&self->mutex);
}

static gboolean texture_rgba_is_terminate(TextureRgba* texture) {
  TextureRgbaPrivate *self = (TextureRgbaPrivate *)
      texture_rgba_get_instance_private(TEXTURE_RGBA_RENDERER_TEXTURE_RGBA(texture));
  return g_atomic_int_get(&self->terminated);
}

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
    g_atomic_pointer_set(&self->buffer, nullptr);
  }
  else
  {
    glBindTexture(GL_TEXTURE_2D, self->texture_id);
  }
  // For example, we render pixel buffer here.
  // Note that Flutter only accepts textures in GL_RGBA8 format.
  if (g_atomic_pointer_get(&self->buffer)) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, g_atomic_int_get(&self->video_width), g_atomic_int_get(&self->video_height), 0, GL_BGRA,
               GL_UNSIGNED_BYTE, g_atomic_pointer_get(&self->buffer));
  }
  // Now we can release the self->buffer.
  if (g_atomic_pointer_get(&self->buffer) != nullptr) {
    delete[] self->buffer;
    g_atomic_pointer_set(&self->buffer, nullptr);
  }
  g_atomic_int_set(&self->buffer_ready, FALSE);
  *target = GL_TEXTURE_2D;
  *name = self->texture_id;
  *width = g_atomic_int_get(&self->video_width);
  *height = g_atomic_int_get(&self->video_height);
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