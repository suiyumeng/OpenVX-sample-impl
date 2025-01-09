/* 

 * Copyright (c) 2012-2017 The Khronos Group Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <my_target.h>
#include <vx_debug.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define LUT_SIZE 256
// 3D LUT结构体
typedef struct {
    unsigned char data[LUT_SIZE][LUT_SIZE][LUT_SIZE][3];
} ThreeDLUT;
void generate3DLUT(ThreeDLUT *lut) {
    for (int r = 0; r < LUT_SIZE; r++) {
        for (int g = 0; g < LUT_SIZE; g++) {
            for (int b = 0; b < LUT_SIZE; b++) {
                // 对每个通道应用简单的二次函数进行调整
                int newR = (int)(r * r / 256.0);
                int newG = (int)(g * g / 256.0);
                int newB = (int)(b * b / 256.0);
                // 确保值在0 - 255范围内
                lut->data[r][g][b][0] = (newR > 255)? 255 : newR;
                lut->data[r][g][b][1] = (newG > 255)? 255 : newG;
                lut->data[r][g][b][2] = (newB > 255)? 255 : newB;
            }
        }
    }
}
vx_status vx3DLUT(vx_image src, vx_image dst)
{
    vx_imagepatch_addressing_t src_addr, dst_addr;
    void *src_base = NULL;
    void *dst_base = NULL;
    vx_uint32 y, x, p;
    vx_df_image src_format, dst_format;
    vx_size src_planes, dst_planes;
    vx_enum src_space;
    vx_rectangle_t rect;
    ThreeDLUT *lut3d = NULL;
    vx_status status = VX_SUCCESS;
    if (src == NULL || dst == NULL)
        return VX_ERROR_INVALID_PARAMETERS;
    status |= vxGetValidRegionImage(src, &rect);
    VX_PRINT(VX_ZONE_INFO, "vx3DLUT rect.end_x=%u, rect.end_y=%u\n", rect.end_x, rect.end_y);
    status |= vxQueryImage(src, VX_IMAGE_FORMAT, &src_format, sizeof(src_format));
    status |= vxQueryImage(src, VX_IMAGE_PLANES, &src_planes, sizeof(src_planes));
    if (status != VX_SUCCESS) {
        VX_PRINT(VX_ZONE_ERROR, "vxQueryImage src failed status=%d format=%d src_planes=%d\n", status, src_format, src_planes);
        goto exit;
    }
    status |= vxQueryImage(dst, VX_IMAGE_FORMAT, &dst_format, sizeof(dst_format));
    status |= vxQueryImage(dst, VX_IMAGE_PLANES, &dst_planes, sizeof(dst_planes));
    if (status != VX_SUCCESS) {
        VX_PRINT(VX_ZONE_ERROR, "vxQueryImage dst failed status=%d format=%d src_planes=%d\n", status, dst_format, dst_planes);
        goto exit;
    }
    status |= vxAccessImagePatch(src, &rect, 0, &src_addr, &src_base, VX_READ_ONLY);
    ownPrintImageAddressing(&src_addr);
    if (status != VX_SUCCESS) {
        VX_PRINT(VX_ZONE_ERROR, "vxAccessImagePatch failed status=%d\n", status);
        goto exit;
    }
    status |= vxAccessImagePatch(dst, &rect, 0, &dst_addr, &dst_base, VX_WRITE_ONLY);
    ownPrintImageAddressing(&dst_addr);
    if (status != VX_SUCCESS) {
        VX_PRINT(VX_ZONE_ERROR, "vxAccessImagePatch failed status=%d\n", status);
        goto exit;
    }
    VX_PRINT(VX_ZONE_INFO, "vx3DLUT src_planes=%u, dst_planes=%u\n", src_planes, dst_planes);
    lut3d = (ThreeDLUT *)malloc(sizeof(ThreeDLUT));
    generate3DLUT(lut3d);
    for (y = 0; y < rect.end_y; y++) {
        for (x = 0; x < rect.end_x; x++) {
            //VX_PRINT(VX_ZONE_INFO, "vx3DLUT x=%u, y=%u\n", x, y);
            vx_uint8 *src_rgb = vxFormatImagePatchAddress2d(src_base, x, y, &src_addr);
            vx_uint8 *dst_rgb = vxFormatImagePatchAddress2d(dst_base, x, y, &dst_addr);
            dst_rgb[0] = lut3d->data[src_rgb[0]][src_rgb[1]][src_rgb[2]][0];
            dst_rgb[1] = lut3d->data[src_rgb[0]][src_rgb[1]][src_rgb[2]][1];
            dst_rgb[2] = lut3d->data[src_rgb[0]][src_rgb[1]][src_rgb[2]][2];
        }
    }
    VX_PRINT(VX_ZONE_INFO, "vx3DLUT done\n");
    status |= vxCommitImagePatch(src, &rect, 0, &src_addr, src_base);
    status |= vxCommitImagePatch(dst, &rect, 0, &dst_addr, dst_base);
    if (status != VX_SUCCESS) {
        VX_PRINT(VX_ZONE_ERROR, "vxCommitImagePatch failed status=%d\n", status);
        goto exit;
    }
exit:
    VX_PRINT(VX_ZONE_API, "returned %d\n", status);
    if (lut3d) {
        free(lut3d);
    }
    return status;
}

