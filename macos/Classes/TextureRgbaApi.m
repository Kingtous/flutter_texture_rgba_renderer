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
void FlutterRgbaRendererPluginOnRgba(void* texture_rgba_ptr, const uint8_t* buffer, int len, int width, int height, int row_align_bytes) {
    TextRgba* texture_rgba = (__bridge TextRgba *)(texture_rgba_ptr);
    NSData* data = [NSData dataWithBytesNoCopy:(void*)buffer length:len freeWhenDone:FALSE];
    [texture_rgba markFrameAvaliableWithData:data width:width height:height row_align_bytes: row_align_bytes];
}

#if __cplusplus
} //Extern C
#endif
