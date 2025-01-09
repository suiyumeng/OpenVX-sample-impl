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
 * \brief The C-Model Target Interface
 * \author Erik Rainey <erik.rainey@gmail.com>
 */

#include <VX/vx.h>
#include <VX/vx_helper.h>

#include "vx_internal.h"
#include <vx_interface.h>

static const vx_char name[VX_MAX_TARGET_NAME] = "mytarget.any";

/*! \brief Declares the list of all supported base kernels.
 * \ingroup group_implementation
 * \note This is the list of all supported base kernels! It must at least
 * match the OpenVX 1.0 Specification.
 */
static vx_kernel_description_t *target_kernels[] =
{
    &_3dlut_kernel,
};

/*! \brief Declares the number of base supported kernels.
 * \ingroup group_implementation
 */
static vx_uint32 num_target_kernels = dimof(target_kernels);

/******************************************************************************/
/* EXPORTED FUNCTIONS */
/******************************************************************************/

vx_status vxTargetInit(vx_target target)
{
    if (target)
    {
        strncpy(target->name, name, VX_MAX_TARGET_NAME);
        target->priority = VX_TARGET_PRIORITY_MY_TARGET;
    }
    return ownInitializeTarget(target, target_kernels, num_target_kernels);
}

vx_status vxTargetDeinit(vx_target target)
{
    return ownDeinitializeTarget(target);
}

vx_status vxTargetSupports(vx_target target,
                           vx_char targetName[VX_MAX_TARGET_NAME],
                           vx_char kernelName[VX_MAX_KERNEL_NAME],
                           vx_uint32 *pIndex)
{
    vx_status status = VX_ERROR_NOT_SUPPORTED;
    if (strncmp(targetName, name, VX_MAX_TARGET_NAME) == 0 ||
        strncmp(targetName, "default", VX_MAX_TARGET_NAME) == 0 ||
        strncmp(targetName, "power", VX_MAX_TARGET_NAME) == 0 ||
        strncmp(targetName, "performance", VX_MAX_TARGET_NAME) == 0)
    {
        vx_uint32 k = 0u;
        for (k = 0u; k < VX_INT_MAX_KERNELS; k++)
        {
            vx_char targetKernelName[VX_MAX_KERNEL_NAME];
            vx_char *kernel;
            vx_char def[8] = "default";

            strncpy(targetKernelName, target->kernels[k].name, VX_MAX_KERNEL_NAME);
            kernel = strtok(targetKernelName, ":");
            if (kernel == NULL)
                kernel = def;

            if (strncmp(kernelName, kernel, VX_MAX_KERNEL_NAME) == 0
                )
            {
                status = VX_SUCCESS;
                if (pIndex) *pIndex = k;
                break;
            }
        }
    }
    return status;
}

vx_action vxTargetProcess(vx_target target, vx_node_t *nodes[], vx_size startIndex, vx_size numNodes)
{
    vx_action action = VX_ACTION_CONTINUE;
    vx_status status = VX_SUCCESS;
    vx_size n = 0;
    (void)target;

    for (n = startIndex; (n < (startIndex + numNodes)) && (action == VX_ACTION_CONTINUE); n++)
    {
        vx_context context = vxGetContext((vx_reference)nodes[n]);
        VX_PRINT(VX_ZONE_GRAPH, "Executing Kernel %s:%d in Nodes[%u] on target %s\n",
            nodes[n]->kernel->name,
            nodes[n]->kernel->enumeration,
            n,
            nodes[n]->base.context->targets[nodes[n]->affinity].name);

        if (context->perf_enabled)
            ownStartCapture(&nodes[n]->perf);

        if (nodes[n]->is_replicated == vx_true_e)
        {
            vx_size num_replicas = 0;
            vx_uint32 param;
            vx_uint32 num_parameters = nodes[n]->kernel->signature.num_parameters;
            vx_reference parameters[VX_INT_MAX_PARAMS] = { NULL };

            for (param = 0; param < num_parameters; ++param)
            {
                if (nodes[n]->replicated_flags[param] == vx_true_e)
                {
                    vx_size numItems = 0;
                    if ((nodes[n]->parameters[param])->scope->type == VX_TYPE_PYRAMID)
                    {
                        vx_pyramid pyr = (vx_pyramid)(nodes[n]->parameters[param])->scope;
                        numItems = pyr->numLevels;
                    }
                    else if ((nodes[n]->parameters[param])->scope->type == VX_TYPE_OBJECT_ARRAY)
                    {
                        vx_object_array arr = (vx_object_array)(nodes[n]->parameters[param])->scope;
                        numItems = arr->num_items;
                    }
                    else
                    {
                        status = VX_ERROR_INVALID_PARAMETERS;
                        break;
                    }

                    if (num_replicas == 0)
                        num_replicas = numItems;
                    else if (numItems != num_replicas)
                    {
                        status = VX_ERROR_INVALID_PARAMETERS;
                        break;
                    }
                }
                else
                {
                    parameters[param] = nodes[n]->parameters[param];
                }
            }

            if (status == VX_SUCCESS)
            {
                vx_size replica;
                for (replica = 0; replica < num_replicas; ++replica)
                {
                    for (param = 0; param < num_parameters; ++param)
                    {
                        if (nodes[n]->replicated_flags[param] == vx_true_e)
                        {
                            if ((nodes[n]->parameters[param])->scope->type == VX_TYPE_PYRAMID)
                            {
                                vx_pyramid pyr = (vx_pyramid)(nodes[n]->parameters[param])->scope;
                                parameters[param] = (vx_reference)pyr->levels[replica];
                            }
                            else if ((nodes[n]->parameters[param])->scope->type == VX_TYPE_OBJECT_ARRAY)
                            {
                                vx_object_array arr = (vx_object_array)(nodes[n]->parameters[param])->scope;
                                parameters[param] = (vx_reference)arr->items[replica];
                            }
                        }
                    }

                    status = nodes[n]->kernel->function((vx_node)nodes[n],
                        parameters,
                        num_parameters);
                }
            }
        }
        else
        {
            status = nodes[n]->kernel->function((vx_node)nodes[n],
                (vx_reference *)nodes[n]->parameters,
                nodes[n]->kernel->signature.num_parameters);
        }

        nodes[n]->executed = vx_true_e;
        nodes[n]->status = status;

        if (context->perf_enabled)
            ownStopCapture(&nodes[n]->perf);

        VX_PRINT(VX_ZONE_GRAPH, "kernel %s returned %d\n", nodes[n]->kernel->name, status);

        if (status == VX_SUCCESS)
        {
            /* call the callback if it is attached */
            if (nodes[n]->callback)
            {
                action = nodes[n]->callback((vx_node)nodes[n]);
                VX_PRINT(VX_ZONE_GRAPH, "callback returned action %d\n", action);
            }
        }
        else
        {
            action = VX_ACTION_ABANDON;
            VX_PRINT(VX_ZONE_ERROR, "Abandoning Graph due to error (%d)!\n", status);
        }
    }
    return action;
}

vx_status vxTargetVerify(vx_target target, vx_node_t *node)
{
    vx_status status = VX_SUCCESS;
    (void)target;
    (void)node;

    return status;
}

vx_kernel vxTargetAddKernel(vx_target target,
                            vx_char name[VX_MAX_KERNEL_NAME],
                            vx_enum enumeration,
                            vx_kernel_f func_ptr,
                            vx_uint32 numParams,
                            vx_kernel_validate_f validate,
                            vx_kernel_input_validate_f input,
                            vx_kernel_output_validate_f output,
                            vx_kernel_initialize_f initialize,
                            vx_kernel_deinitialize_f deinitialize)
{
    vx_uint32 k = 0u;
    vx_kernel_t *kernel = NULL;
    // ownSemWait(&target->base.lock);
    for (k = 0; k < VX_INT_MAX_KERNELS; k++)
    {
        kernel = &(target->kernels[k]);
        if (kernel->enabled == vx_false_e)
        {
            ownInitializeKernel(target->base.context,
                               kernel,
                               enumeration, func_ptr, name,
                               NULL, numParams,
                               validate, input, output, initialize, deinitialize);
            VX_PRINT(VX_ZONE_KERNEL, "Reserving %s Kernel[%u] for %s\n", target->name, k, kernel->name);
            target->num_kernels++;
            break;
        }
        kernel = NULL;
    }
    // ownSemPost(&target->base.lock);
    return (vx_kernel)kernel;
}


