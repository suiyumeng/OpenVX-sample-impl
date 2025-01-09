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
 * \brief The 3D Look Up Kernels
 * \author Sui Yumeng <suiyumeng@bytedance.com>
 */

#include <VX/vx.h>
#include <VX/vx_helper.h>

#include "vx_internal.h"
#include <VX/vx_mytarget.h>
#include <my_target.h>

static vx_status VX_CALLBACK vx3DLUTKernel(vx_node node, const vx_reference *parameters, vx_uint32 num)
{
    (void)node;

    if (num == 2)
    {
        vx_image src = (vx_image)parameters[0];
        vx_image dst = (vx_image)parameters[1];

        return vx3DLUT(src, dst);
    }
    return VX_ERROR_INVALID_PARAMETERS;
}

static vx_status VX_CALLBACK vx3DLUTInputValidator(vx_node node, vx_uint32 index)
{
   vx_status status = VX_SUCCESS;
    if (index == 0)
    {
        vx_parameter param = vxGetParameterByIndex(node, index);
        if (vxGetStatus((vx_reference)param) == VX_SUCCESS)
        {
            vx_image image = 0;
            vxQueryParameter(param, VX_PARAMETER_REF, &image, sizeof(image));
            if (image)
            {
                vx_df_image format = 0;
                vxQueryImage(image, VX_IMAGE_FORMAT, &format, sizeof(format));
                // check to make sure the input format is supported.
                switch (format)
                {
                    case VX_DF_IMAGE_RGB:  /* 8:8:8 interleaved */
                    case VX_DF_IMAGE_RGBX: /* 8:8:8:8 interleaved */
                        break;
                    default:
                        status = VX_ERROR_INVALID_FORMAT;
                        VX_PRINT(VX_ZONE_ERROR, "3DLUT: unsupported format=0x%x %c%c%c%c\n", format, format & 0xff, (format >> 8) & 0xff, (format >> 16) & 0xff, (format >> 24) & 0xff);
                        break;
                }
                vxReleaseImage(&image);
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
                VX_PRINT(VX_ZONE_ERROR, "3DLUT: invalid input parameter\n");
            }
            vxReleaseParameter(&param);
        }
        else
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "3DLUT: invalid input parameter. index=%u", index);
        }
    }
    else
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        VX_PRINT(VX_ZONE_ERROR, "3DLUT: invalid input parameter index=%u", index);
    }
    return status;
}

static vx_status VX_CALLBACK vx3DLUTOutputValidator(vx_node node, vx_uint32 index, vx_meta_format_t *ptr)
{
   vx_status status = VX_SUCCESS;
    if (index == 1)
    {
        vx_parameter param = vxGetParameterByIndex(node, index);
        if (vxGetStatus((vx_reference)param) == VX_SUCCESS)
        {
            vx_image image = 0;
            vxQueryParameter(param, VX_PARAMETER_REF, &image, sizeof(image));
            if (image)
            {
                vx_df_image format = 0;
                vx_uint32 width = 0, height = 0;

                vxQueryImage(image, VX_IMAGE_FORMAT, &format, sizeof(format));
                vxQueryImage(image, VX_IMAGE_WIDTH, &width, sizeof(width));
                vxQueryImage(image, VX_IMAGE_HEIGHT, &height, sizeof(height));
                ptr->type = VX_TYPE_IMAGE;
                ptr->dim.image.format = format;
                ptr->dim.image.width = width;
                ptr->dim.image.height = height;
                // check to make sure the input format is supported.
                switch (format)
                {
                    case VX_DF_IMAGE_RGB:  /* 8:8:8 interleaved */
                    case VX_DF_IMAGE_RGBX: /* 8:8:8:8 interleaved */
                        break;
                    default:
                        status = VX_ERROR_INVALID_FORMAT;
                        VX_PRINT(VX_ZONE_ERROR, "3DLUT: unsupported format=0x%x %c%c%c%c\n", format, format & 0xff, (format >> 8) & 0xff, (format >> 16) & 0xff, (format >> 24) & 0xff);
                        break;
                }
                vxReleaseImage(&image);
            }
            else
            {
                status = VX_ERROR_INVALID_PARAMETERS;
                VX_PRINT(VX_ZONE_ERROR, "3DLUT: invalid output parameter\n");
            }
            vxReleaseParameter(&param);
        }
        else
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            VX_PRINT(VX_ZONE_ERROR, "3DLUT: invalid output parameter\n");
        }
    }
    else
    {
        status = VX_ERROR_INVALID_PARAMETERS;
        VX_PRINT(VX_ZONE_ERROR, "3DLUT: invalid output parameter index=%u\n", index);

    }
    return status;
}


/*! \brief Declares the parameter types for \ref vxuColorConvert.
 * \ingroup group_implementation
 */
static vx_param_description_t _3dlut_kernel_params[] = {
    {VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
    {VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED},
};

/*! \brief The exported kernel table entry */
vx_kernel_description_t _3dlut_kernel = {
    VX_KERNEL_3DLUT,
    "org.khronos.openvx.3dlut",
    vx3DLUTKernel,
    _3dlut_kernel_params, dimof(_3dlut_kernel_params),
    NULL,
    vx3DLUTInputValidator,
    vx3DLUTOutputValidator,
    NULL,
    NULL,
};

VX_API_ENTRY vx_node VX_API_CALL vx3DLUTNode(vx_graph graph, vx_image input, vx_image output)
{
    vx_reference params[] = {
        (vx_reference)input,
        (vx_reference)output,
    };
    return vxCreateNodeByStructure(graph, VX_KERNEL_3DLUT, params, dimof(params));
}