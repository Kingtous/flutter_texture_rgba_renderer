#include "texture_rgba.h"

TextureRgba::TextureRgba(flutter::TextureRegistrar* texture_registrar)
{
	this->texture_registrar_ = texture_registrar;
	// TODO: flutter only support PixelBuffer in 2023.02.17.
    auto buffer = flutter::PixelBufferTexture([=](size_t width, size_t height) -> const FlutterDesktopPixelBuffer* {
		return this->buffer();
        });
	texture_ = std::make_unique<flutter::TextureVariant>(buffer);
	this->texture_id_ = texture_registrar->RegisterTexture(texture_.get());
}

TextureRgba::~TextureRgba()
{
	texture_registrar_->UnregisterTexture(texture_id_);
}

void TextureRgba::MarkVideoFrameAvailable(const std::vector<uint8_t>& buffer, size_t width, size_t height)
{
	const std::lock_guard<std::mutex> lock(mutex_);
	// FIXME: remove copy.
	buffer_tmp_.resize(buffer.size());
	std::copy(buffer.begin(), buffer.end(), buffer_tmp_.begin());
	flutter_pixel_buffer_.buffer = &buffer_tmp_[0];
	flutter_pixel_buffer_.width = width;
	flutter_pixel_buffer_.height = height;
	texture_registrar_->MarkTextureFrameAvailable(texture_id_);
}

inline const FlutterDesktopPixelBuffer* TextureRgba::buffer()
{
	return &this->flutter_pixel_buffer_;
}
