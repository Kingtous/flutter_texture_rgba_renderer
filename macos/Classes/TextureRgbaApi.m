//
//  TextureRgbaApi.m
//  texture_rgba_renderer
//
//  Created by kingtous on 2023/2/19.
//

#import <Foundation/Foundation.h>
#import "texture_rgba_renderer/texture_rgba_renderer-Swift.h"

#if __cplusplus
extern "C" {
#endif
    
/// Keep the same name with Windows.
void FlutterRgbaRendererPluginOnRgba(void* texture_rgba_ptr, const uint8_t* buffer, int width, int height, int stride_align) {
    TextRgba* texture_rgba = (__bridge TextRgba *)(texture_rgba_ptr);
    NSData* data = [NSData dataWithBytesNoCopy:(void*)buffer length:((width + stride_align - 1) & ~(stride_align - 1)) * height * 4 freeWhenDone:FALSE];
    [texture_rgba markFrameAvaliableWithData:data width:width height:height stride_align: stride_align];
}

#if __cplusplus
} //Extern C
#endif
