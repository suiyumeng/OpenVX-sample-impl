#include "base.h"
#include <VX/vx.h>
#include <VX/vxu.h>
#include <VX/vx_compatibility.h>
#include <VX/vx_lib_extras.h>
#include <opencv2/opencv.hpp>


vx_status vx_test_lib_ext_laplacian3x3(int argc, char *argv[])
{
    vx_status status = VX_SUCCESS;
    unsigned long int i;
    char *image_path = NULL;
    vx_rectangle_t rect;
    if (argc >= 4) {
        image_path = argv[3];
    }
    if (image_path == NULL) {
        LOG_E("image_path is NULL\n");
        LOG_E("Usage:  <image_path>\n");
        return VX_ERROR_INVALID_PARAMETERS;
    }
    LOG_D("image_path=%s\n", image_path);
    // 使用openCV读取图像
    cv::Mat img_input = cv::imread(image_path, cv::IMREAD_GRAYSCALE); // 读取灰度图像

    if (img_input.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return VX_ERROR_INVALID_PARAMETERS;
    }
    LOG_D("%s: image.rows=%d, image.cols=%d channels=%d type=%d step[0]=%lu\n",
        image_path, img_input.rows, img_input.cols, img_input.channels(), img_input.type(), img_input.step[0]);
    vx_uint32 w = img_input.cols;
    vx_uint32 h = img_input.rows;
    vx_context context = vxCreateContext();

    if (vxGetStatus((vx_reference)context) == VX_SUCCESS)
    {
        // 创建图
        vx_graph graph = vxCreateGraph(context);
        if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
        {
                // 利用opencv图像信息配置openvx图像地址结构
            vx_imagepatch_addressing_t addr = {w, h, (vx_int32)(sizeof(vx_uint8)*img_input.channels()), (vx_int32)(w * sizeof(vx_uint8)*img_input.channels()), VX_SCALE_UNITY, VX_SCALE_UNITY, 1, 1};

            vx_image images[] = {
                vxCreateImageFromHandle(context, VX_DF_IMAGE_U8, &addr, (void * const *)&img_input.data, VX_MEMORY_TYPE_HOST),
                vxCreateImage(context, w, h, VX_DF_IMAGE_U8),
                //vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_VIRT),
            };

            vxGetValidRegionImage(images[0], &rect);
            vx_node node = 0;

            status = vxLoadKernels(context, "openvx-extras");
            node = vxLaplacian3x3Node(graph, images[0], images[1]);
            if (node)
            {
                status = vxVerifyGraph(graph);
                if (status == VX_SUCCESS)
                {
                    status = vxProcessGraph(graph);
                    if (status == VX_SUCCESS) {
                        LOG_I("Graph Execution Successful\n");
                        // 保存输出图像（这里假设已经有保存路径
                        vx_map_id map_id;
                        void *ptr_out;
                        int out_img_id = dimof(images) - 1;
                        //VX_API_ENTRY vx_status VX_API_CALL vxMapImagePatch(vx_image image, const vx_rectangle_t *rect, vx_uint32 plane_index, vx_map_id *map_id, vx_imagepatch_addressing_t *addr, void **ptr, vx_enum usage, vx_enum mem_type, vx_uint32 flags);
                        status = vxMapImagePatch(images[out_img_id], &rect, 0, &map_id, &addr, &ptr_out, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X);
                        if (status == VX_SUCCESS) {
                            // 此时data_ptr指向vx_image的数据，可以进行转换操作
                            cv::Mat img_out(h, w, CV_8UC1, ptr_out); // CV_8UC1 表示单通道8位无符号整数类型, 1 表示通道数量,
                            cv::imwrite("output.jpg", img_out);
                            LOG_I("output.jpg saved\n");
                            // 解除映射
                            status = vxUnmapImagePatch(images[out_img_id], map_id);
                            if (status != VX_SUCCESS) {
                                LOG_E("vxUnmapImagePatch failed\n");
                            }
                        } else {
                            LOG_E("vxMapImagePatch failed\n");
                        }
                    } else {
                        LOG_E("Graph Execution Failed\n");
                    }
                }
                else
                {
                    LOG_E("Failed Graph Verification\n");
                }
                vxReleaseNode(&node);
            }
            else
            {
                LOG_E("failed to create node\n");
                status |= VX_ERROR_NOT_SUFFICIENT;
            }
            for (i = 0; i < dimof(images); i++)
            {
                vxReleaseImage(&images[i]);
            }
            status |= vxUnloadKernels(context, "openvx-extras");
        }
        else
        {
            LOG_E("failed to create graph\n");
            status |= VX_ERROR_NOT_SUFFICIENT;
        }

        vxReleaseContext(&context);
    }
    return status;
}