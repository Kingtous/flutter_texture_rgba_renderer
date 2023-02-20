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
	int bg_index = 0;
	{
		const std::lock_guard<std::mutex> lock(mutex_);
		if (!pixel_buff_set_ && last_fg_index_ != fg_index_)
		{
			if (buffer_tmp_[fg_index_].empty())
			{
				flutter_pixel_buffer_.buffer = nullptr;
				flutter_pixel_buffer_.width = 0;
				flutter_pixel_buffer_.height = 0;
			}
			else
			{
				flutter_pixel_buffer_.buffer = static_cast<const uint8_t *>(buffer_tmp_[fg_index_].data());
				flutter_pixel_buffer_.width = width_[fg_index_];
				flutter_pixel_buffer_.height = height_[fg_index_];
			}
			last_fg_index_ = fg_index_;
			pixel_buff_set_ = true;
			texture_registrar_->MarkTextureFrameAvailable(texture_id_);
		}
		bg_index = fg_index_ ^ 1;
	}

	buffer.swap(buffer_tmp_[bg_index]);
	width_[bg_index] = width;
	height_[bg_index] = height;
}

// This function may be called
// event texture_registrar_->MarkTextureFrameAvailable(texture_id_); is not called before.
inline const FlutterDesktopPixelBuffer *TextureRgba::buffer()
{
	const std::lock_guard<std::mutex> lock(mutex_);
	if (pixel_buff_set_)
	{
		fg_index_ ^= 1;
		pixel_buff_set_ = false;
	}
	return &this->flutter_pixel_buffer_;
}
