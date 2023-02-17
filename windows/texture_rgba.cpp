#include "texture_rgba.h"

TextureRgba::TextureRgba(flutter::TextureRegistrar* texture_registrar)
{
}

TextureRgba::~TextureRgba()
{
	texture_registrar_->UnregisterTexture(texture_id_);
}

void TextureRgba::MarkVideoFrameAvailable(uint8_t* buffer, int32_t width, int32_t height)
{
	const std::lock_guard<std::mutex> lock(mutex_);
	flutter_pixel_buffer_.buffer = buffer;
	flutter_pixel_buffer_.width = width;
	flutter_pixel_buffer_.height = height;
	texture_registrar_->MarkTextureFrameAvailable(texture_id_);
}
