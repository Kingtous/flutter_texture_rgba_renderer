#ifndef TEXTURE_RGBA_H_
#define TEXTURE_RGBA_H_

#include <flutter/texture_registrar.h>
#include <mutex>
#include <chrono>

class TextureRgba
{
public:
	TextureRgba(flutter::TextureRegistrar* texture_registrar);
	~TextureRgba();

	void MarkVideoFrameAvailable(std::vector<uint8_t>& buffer, size_t width, size_t height);

	int64_t texture_id() const { return texture_id_; };

	const FlutterDesktopPixelBuffer* buffer();

private:
	FlutterDesktopPixelBuffer flutter_pixel_buffer_{};
	flutter::TextureRegistrar* texture_registrar_ = nullptr;
	std::unique_ptr<flutter::TextureVariant> texture_ = nullptr;
	int64_t texture_id_;
	mutable std::mutex mutex_;
	bool pixel_buff_set_ = false;
	int last_fg_index_ = 1;
	int fg_index_ = 0;
	size_t width_[2] = {0, 0};
	size_t height_[2] = {0, 0};
	std::vector<uint8_t> buffer_tmp_[2];
	Timer timer_;
};

#endif  // TEXTURE_RGBA_H_
