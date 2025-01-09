#include "base.h"
#include <opencv2/opencv.hpp>
#include <VX/vx.h>
#include <VX/vxu.h>
#include <VX/vx_compatibility.h>
#include <VX/vx_mytarget.h>
vx_status vx_test_3dlut(int argc, char *argv[])
{
    vx_status status = VX_SUCCESS;
    unsigned long int i;
    char *image_path = NULL;
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
    cv::Mat img_input = cv::imread(image_path);

    if (img_input.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return VX_ERROR_INVALID_PARAMETERS;
    }
    LOG_D("%s: image.rows=%d, image.cols=%d channels=%d type=%d step[0]=%lu\n",
        image_path, img_input.rows, img_input.cols, img_input.channels(), img_input.type(), img_input.step[0]);
    vx_uint32 w = img_input.cols;
    vx_uint32 h = img_input.rows;
    vx_context context = vxCreateContext();
    if (context == NULL) {
        LOG_E("无法创建OpenVX上下文\n");
        return VX_ERROR_INVALID_REFERENCE;
    }

    if (vxGetStatus((vx_reference)context) != VX_SUCCESS) {
        vxReleaseContext(&context);
        LOG_E("context status is not success!");
        return VX_ERROR_INVALID_REFERENCE;
    }
    // 利用opencv图像信息配置openvx图像地址结构
    vx_imagepatch_addressing_t addr = {w, h, (vx_int32)(sizeof(vx_uint8)*img_input.channels()), (vx_int32)(w * sizeof(vx_uint8)*img_input.channels()), VX_SCALE_UNITY, VX_SCALE_UNITY, 1, 1};

    vx_image images[] = {
        vxCreateImage(context, w, h, VX_DF_IMAGE_RGB),
        vxCreateImage(context, w, h, VX_DF_IMAGE_RGB),
    };

    images[0] = vxCreateImageFromHandle(context, VX_DF_IMAGE_RGB, &addr, (void * const *)&img_input.data, VX_MEMORY_TYPE_HOST);
    // 创建图
    vx_graph graph = vxCreateGraph(context);
    if (vxGetStatus((vx_reference)graph) == VX_SUCCESS) {
        LOG_I("vxCreateGraph success\n");
        vx_node nodes[] = {
            //vxColorConvertNode(graph, images[0], images[1]),
            vx3DLUTNode(graph, images[0], images[1]),
            //vxGaussian3x3Node(graph, images[0], images[1]),
            //vxBox3x3Node(graph, images[0], images[1]),
            //vxMedian3x3Node(graph, images[0], images[1]),
            //vxSobel3x3Node(graph, images[0], images[1]),
            //vxSobel5x5Node(graph, images[0], images[1]),
            //vxColorConvertNode(graph, images[0], images[1]),
        };
        status = vxVerifyGraph(graph);
        if (status == VX_SUCCESS) {
            LOG_I("vxVerifyGraph success\n");
            status = vxProcessGraph(graph);
            if (status == VX_SUCCESS) {
                LOG_I("vxProcessGraph success\n");
                // 保存输出图像（这里假设已经有保存路径
                vx_map_id map_id;
                vx_rectangle_t rect = {0, 0, w, h};
                void *ptr_out;
                int out_img_id = dimof(images) - 1;
                //VX_API_ENTRY vx_status VX_API_CALL vxMapImagePatch(vx_image image, const vx_rectangle_t *rect, vx_uint32 plane_index, vx_map_id *map_id, vx_imagepatch_addressing_t *addr, void **ptr, vx_enum usage, vx_enum mem_type, vx_uint32 flags);
                status = vxMapImagePatch(images[out_img_id], &rect, 0, &map_id, &addr, &ptr_out, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X);
                if (status == VX_SUCCESS) {
                    // 此时data_ptr指向vx_image的数据，可以进行转换操作
                    //cv::Mat img_out(h, w, CV_8UC1, ptr_out); // CV_8UC1 表示单通道8位无符号整数
                    cv::Mat img_out(h, w, CV_8UC3, ptr_out); // CV_8UC3 表示3通道8位无符号整数
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
                LOG_E("vxProcessGraph failed\n");
            }
            
            
        } else {
            LOG_E("vxVerifyGraph failed\n");
        }
        for (i = 0; i < dimof(nodes); i++)
        {
            vxReleaseNode(&nodes[i]);
        }
        vxReleaseGraph(&graph);
    } else {
        LOG_E("vxCreateGraph failed\n");
    }
    for (i = 0; i < dimof(images); i++)
    {
        vxReleaseImage(&images[i]);
    }

    vxReleaseContext(&context);
    return status;
}