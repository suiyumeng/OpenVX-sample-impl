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

#ifndef _OPENVX_EXT_DEBUG_H_
#define _OPENVX_EXT_DEBUG_H_

#include <VX/vx.h>


/*! \brief The maximum filepath name length.
 * \ingroup group_debug_ext
 */
#define VX_MAX_FILE_NAME    (256)

/*! \brief The library value for the extension
 * \ingroup group_debug_ext
 */
#define VX_LIBRARY_MY (0xAE)

/*! \brief The list of extensions to OpenVX from the Sample Implementation.
 * \ingroup group_debug_ext
 */
enum vx_kernel_debug_ext_e {

    /*!
     * \brief The Copy kernel. Output = Input.
     * \param  [in] vx_image The input image.
     * \param [out] vx_image The output image.
     * \see group_vision_function_copy_image
     */
    VX_KERNEL_MY_MM = VX_KERNEL_BASE(VX_ID_KHRONOS, VX_LIBRARY_MY) + 0x0,
    VX_KERNEL_MY_ROTATION = VX_KERNEL_BASE(VX_ID_KHRONOS, VX_LIBRARY_MY) + 0x1,
};

enum vx_rotation_type_e {
    VX_ROTATION_90 = 0,
    VX_ROTATION_180 = 1,
    VX_ROTATION_270 = 2,
};

/******************************************************************************/
// GRAPH MODE FUNCTIONS
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
/*!
 * \brief [Graph] Creates a Copy Image Node.
 * \param [in] graph The handle to the graph.
 * \param [in] input The input image.
 * \param [out] output The output image.
 * \see VX_KERNEL_COPY_IMAGE
 * \note Graph Mode Function.
 * \ingroup group_vision_function_copy_image
 */
vx_node vxMMNode(vx_graph graph, vx_image input, vx_image output);
vx_node vxMyRotationNode(vx_graph graph, vx_image input, enum vx_rotation_type_e angle, vx_image output);

#ifdef __cplusplus
}
#endif

#endif

