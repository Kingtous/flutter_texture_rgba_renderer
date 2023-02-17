#include <flutter/texture_registrar.h>
#include <mutex>

#pragma once
class TextureRgba
{
public:
	TextureRgba(flutter::TextureRegistrar* texture_registrar);
	~TextureRgba();

	void MarkVideoFrameAvailable(uint8_t* buffer, int32_t width, int32_t height);

	int64_t texture_id() const { return texture_id_; };

private:
	FlutterDesktopPixelBuffer flutter_pixel_buffer_{};
	flutter::TextureRegistrar* texture_registrar_ = nullptr;
	std::unique_ptr<flutter::TextureVariant> texture_ = nullptr;
	int64_t texture_id_;
	mutable std::mutex mutex_;
};

