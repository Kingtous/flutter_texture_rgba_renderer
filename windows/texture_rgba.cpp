#include "texture_rgba.h"
#include <iostream>


TextureRgba::TextureRgba(flutter::TextureRegistrar *texture_registrar)
{
	this->texture_registrar_ = texture_registrar;
	// TODO: flutter only support PixelBuffer in 2023.02.17.
	auto buffer = flutter::PixelBufferTexture([=](size_t width, size_t height) -> const FlutterDesktopPixelBuffer *
											  { return this->buffer(); });
	texture_ = std::make_unique<flutter::TextureVariant>(buffer);
	this->texture_id_ = texture_registrar->RegisterTexture(texture_.get());
}

TextureRgba::~TextureRgba()
{
	texture_registrar_->UnregisterTexture(texture_id_);
}

void TextureRgba::MarkVideoFrameAvailable(
	std::vector<uint8_t> &buffer, size_t width, size_t height)
{
	if (buffer.empty() || buffer.size() != width * height * 4)
	{
		return;
	}

	const std::lock_guard<std::mutex> lock(mutex_);
	int bg_index = fg_index_ ^ 1;
	buffer.swap(buffer_tmp_[bg_index]);
	width_[bg_index] = width;
	height_[bg_index] = height;
	if (!buff_ready_)
	{
		buff_ready_ = true;
		texture_registrar_->MarkTextureFrameAvailable(texture_id_);
	}
}

// This function may be called
// event texture_registrar_->MarkTextureFrameAvailable(texture_id_); is not called before.
inline const FlutterDesktopPixelBuffer *TextureRgba::buffer()
{
	const std::lock_guard<std::mutex> lock(mutex_);
	if (buff_ready_)
	{
		fg_index_ ^= 1;
		flutter_pixel_buffer_.buffer = static_cast<const uint8_t *>(buffer_tmp_[fg_index_].data());
		flutter_pixel_buffer_.width = width_[fg_index_];
		flutter_pixel_buffer_.height = height_[fg_index_];
		buff_ready_ = false;
	}
	return &this->flutter_pixel_buffer_;
}
