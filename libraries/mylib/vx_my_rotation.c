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

/*!
 * \file
 * \brief The Debug Extensions Interface Library.
 * \author Erik Rainey <erik.rainey@gmail.com>
 */

#include <stdio.h>
#include <assert.h>
#include <VX/vx.h>
#include <VX/vx_helper.h>
#include <vx_internal.h>

#include <VX/vx_lib_my.h>

static vx_param_description_t my_rotation_kernel_params[] =
{
    { VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
    { VX_INPUT, VX_TYPE_ENUM, VX_PARAMETER_STATE_REQUIRED },
    { VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED },
};

static vx_status VX_CALLBACK vxRotationKernel(vx_node node, const vx_reference parameters[], vx_uint32 num)
{
    vx_imagepatch_addressing_t src_addr, dst_addr;
    void *src_base = NULL;
    void *dst_base = NULL;
    vx_uint8 *src_pixel = NULL;
    vx_uint8 *dst_pixel = NULL;
    vx_uint32 y, x;
    vx_rectangle_t rect;
    vx_status status = VX_SUCCESS;
    if (num != 3) {
        return VX_ERROR_INVALID_PARAMETERS;
    }
    vx_image src = (vx_image)parameters[0];
    vx_scalar angle_scalar = (vx_scalar)parameters[1];
    vx_image dst = (vx_image)parameters[2];
    if (src == NULL || dst == NULL) {
        return VX_ERROR_INVALID_PARAMETERS;
    }
    enum vx_rotation_type_e angle = 0;
    vxCopyScalar(angle_scalar, &angle, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
    if (status!= VX_SUCCESS) {
        VX_PRINT(VX_ZONE_ERROR, "vxQueryScalar failed status=%d\n", status);
        return status;
    }

    status |= vxGetValidRegionImage(src, &rect);
    VX_PRINT(VX_ZONE_INFO, "rect.end_x=%u, rect.end_y=%u angle=%d\n", rect.end_x, rect.end_y, angle);
    status |= vxAccessImagePatch(src, &rect, 0, &src_addr, &src_base, VX_READ_ONLY);
    status |= vxAccessImagePatch(dst, &rect, 0, &dst_addr, &dst_base, VX_WRITE_ONLY);
    for (y = 0; y < rect.end_y; y++) {
        for (x = 0; x < rect.end_x; x++) {
            if (angle == VX_ROTATION_180) {
                //VX_PRINT(VX_ZONE_INFO, "x=%d, y=%d\n", x, y);
                src_pixel = vxFormatImagePatchAddress2d(src_base, x, y, &src_addr);
                dst_pixel = vxFormatImagePatchAddress2d(dst_base, x, rect.end_y - 1 - y, &dst_addr);
                dst_pixel[0] = src_pixel[0];
                dst_pixel[1] = src_pixel[1];
                dst_pixel[2] = src_pixel[2];
            }
        }
    }
    VX_PRINT(VX_ZONE_INFO, "rotation done\n");
    status |= vxCommitImagePatch(src, &rect, 0, &src_addr, src_base);
    status |= vxCommitImagePatch(dst, &rect, 0, &dst_addr, dst_base);
    if (status != VX_SUCCESS) {
        VX_PRINT(VX_ZONE_ERROR, "vxCommitImagePatch failed status=%d\n", status);
        return status;
    }
    return status;
}

static vx_status VX_CALLBACK my_rotation_validator(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;

    if (NULL != node && NULL != parameters && num == dimof(my_rotation_kernel_params) && NULL != metas)
    {
        vx_parameter param = vxGetParameterByIndex(node, 0); /* input image */
        vx_parameter param2 = vxGetParameterByIndex(node, 2); /* output image */

        if (VX_SUCCESS == vxGetStatus((vx_reference)param))
        {
            vx_image input = 0;

            status = vxQueryParameter(param, VX_PARAMETER_REF, &input, sizeof(vx_image));
#if 1
            if (VX_SUCCESS == status)
            {
                vx_uint32 width = 0;
                vx_uint32 height = 0;
                vx_df_image format = VX_DF_IMAGE_VIRT;

                status |= vxQueryImage(input, VX_IMAGE_WIDTH,  &width,  sizeof(width));
                status |= vxQueryImage(input, VX_IMAGE_HEIGHT, &height, sizeof(height));
                status |= vxQueryImage(input, VX_IMAGE_FORMAT, &format, sizeof(format));
                VX_PRINT(VX_ZONE_INFO, "vxQueryImage: %d, %d, 0x%lx\n", width, height, format);
                if (VX_SUCCESS == status)
                {
                    if (format == VX_DF_IMAGE_RGB) {
                        status |= vxSetMetaFormatAttribute(metas[2], VX_IMAGE_WIDTH,  &width,  sizeof(width)); // 2 : param index, output image
                        status |= vxSetMetaFormatAttribute(metas[2], VX_IMAGE_HEIGHT, &height, sizeof(height)); 
                        status |= vxSetMetaFormatAttribute(metas[2], VX_IMAGE_FORMAT, &format, sizeof(format));
                    } else {
                        VX_PRINT(VX_ZONE_ERROR, "vxQueryImage: invalid format\n");
                        status = VX_ERROR_INVALID_PARAMETERS;
                    }
                } else {
                    VX_PRINT(VX_ZONE_ERROR, "vxQueryImage failed\n");
                    status = VX_ERROR_INVALID_PARAMETERS;
                }
            }
#endif
            if (NULL != input)
                vxReleaseImage(&input);
        }

        if (NULL != param)
            vxReleaseParameter(&param);
    }

    return VX_SUCCESS;
}
vx_kernel_description_t mylib_rotation_kernel =
{
    VX_KERNEL_MY_ROTATION,
    // default target is khronos.any, means c_model
    "org.khronos.mylib.rotation",
    //"pc.opencl:org.khronos.mylib.rotation",
    //"mytarget:org.khronos.mylib.rotation",
    vxRotationKernel,
    my_rotation_kernel_params, dimof(my_rotation_kernel_params),
    my_rotation_validator,
    NULL,
    NULL,
    NULL,
    NULL,
};

vx_node vxMyRotationNode(vx_graph graph, vx_image input, enum vx_rotation_type_e angle, vx_image output)
{
    vx_context context = vxGetContext((vx_reference)graph);
    vx_scalar r_angle = vxCreateScalar(context, VX_TYPE_ENUM, &angle);
    vx_reference params[] = {
        (vx_reference)input,
        (vx_reference)r_angle,
        (vx_reference)output,
    };
    return vxCreateNodeByStructure(graph, VX_KERNEL_MY_ROTATION, params, dimof(params));
}