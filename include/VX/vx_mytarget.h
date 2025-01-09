#ifndef _OPENVX_MY_TARGET_H_
#define _OPENVX_MY_TARGET_H_

#include <VX/vx.h>
#include <VX/vxu.h>

#define VX_LIBRARY_MYTARGET_BASE  0

/*! \brief The list of my-target Kernels.
 * \ingroup group_
 */
enum vx_kernel_my_target_e {
    /*! \brief The 3D LUT kernel.
    * \see group_vision_function_3dlut
    */
    VX_KERNEL_3DLUT = VX_KERNEL_BASE(VX_ID_VIVANTE, VX_LIBRARY_MYTARGET_BASE) + 1,

};


#ifdef __cplusplus
extern "C" {
#endif

/*! \brief [Graph] Creates a 3DLUT node.
 * \param [in] graph The reference to the graph.
 * \param [in] input The input image from which to convert.
 * \param [out] output The output image to which to convert, which must have the same dimensions as the input image.
 * \see <tt>VX_KERNEL_3DLUT</tt>
 * \ingroup group_vision_function_3dlut
 * \return <tt>\ref vx_node</tt>.
 * \retval vx_node A node reference. Any possible errors preventing a successful creation should be checked using <tt>\ref vxGetStatus</tt>
 */
VX_API_ENTRY vx_node VX_API_CALL vx3DLUTNode(vx_graph graph, vx_image input, vx_image output);


#ifdef __cplusplus
}  // extern "C"
#endif

#endif


