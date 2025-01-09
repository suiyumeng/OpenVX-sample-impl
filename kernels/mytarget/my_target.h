/*

 * Copyright (c) 2011-2017 The Khronos Group Inc.
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

#ifndef _VX_MY_TARGET_H_
#define _VX_MY_TARGET_H_

#include <VX/vx.h>
#include <VX/vx_helper.h>
/* TODO: remove vx_compatibility.h after transition period */
#include <VX/vx_compatibility.h>
#include <VX/vx_khr_nn.h>

#include <math.h>
#include <stdbool.h>

#ifdef _MSC_VER
#define C_KERNEL_INLINE _inline
#else
#define C_KERNEL_INLINE inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

vx_status vx3DLUT(vx_image src, vx_image dst);

#ifdef __cplusplus
}
#endif

#endif
