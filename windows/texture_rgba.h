#include <flutter/texture_registrar.h>
#include <mutex>

#pragma once
class TextureRgba
{
public:
	TextureRgba(flutter::TextureRegistrar* texture_registrar);
	~TextureRgba();

	void MarkVideoFrameAvailable(std::vector<uint8_t>& buffer, size_t width, size_t height);

	void MarkVideoFrameAvailablePtr(const uint8_t* buffer, size_t width, size_t height);

	int64_t texture_id() const { return texture_id_; };

	const FlutterDesktopPixelBuffer* buffer();

private:
	FlutterDesktopPixelBuffer flutter_pixel_buffer_{};
	flutter::TextureRegistrar* texture_registrar_ = nullptr;
	std::unique_ptr<flutter::TextureVariant> texture_ = nullptr;
	int64_t texture_id_;
	mutable std::mutex mutex_;
	std::vector<uint8_t> buffer_tmp_;
};

