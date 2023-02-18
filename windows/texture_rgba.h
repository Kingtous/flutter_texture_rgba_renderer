#include <flutter/texture_registrar.h>
#include <mutex>
#include <chrono>

#pragma once

using namespace std;
using namespace std::chrono;
class Timer
{
public:
Timer() : m_begin(high_resolution_clock::now()) {}
void reset() { m_begin = high_resolution_clock::now(); }
int64_t elapsed() const
{
return duration_cast<chrono::milliseconds>(high_resolution_clock::now() - m_begin).count();
}
int64_t elapsed_micro() const
{
return duration_cast<chrono::microseconds>(high_resolution_clock::now() - m_begin).count();
}
int64_t elapsed_nano() const
{
return duration_cast<chrono::nanoseconds>(high_resolution_clock::now() - m_begin).count();
}
int64_t elapsed_seconds() const
{
return duration_cast<chrono::seconds>(high_resolution_clock::now() - m_begin).count();
}
int64_t elapsed_minutes() const
{
return duration_cast<chrono::minutes>(high_resolution_clock::now() - m_begin).count();
}
int64_t elapsed_hours() const
{
return duration_cast<chrono::hours>(high_resolution_clock::now() - m_begin).count();
}
private:
time_point<high_resolution_clock> m_begin;
};


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
	std::vector<uint8_t> buffer_tmp_;
	Timer timer_;
};

