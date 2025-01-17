#include "base.h"
#include <VX/vx.h>
#include <VX/vxu.h>
#include <VX/vx_types.h>

vx_status vx_test_mytarget_3dlut(int argc, char *argv[]);
vx_status vx_test_edge_detection_canny(int argc, char*argv[]);
vx_status vx_test_edge_detection_sobel3x3(int argc, char** argv);
vx_status vx_test_lib_ext_laplacian3x3(int argc, char *argv[]);
vx_status vx_test_mylib_rotation(int argc, char *argv[]);

/*! \brief A local definition to point to a specific unit test */
typedef vx_status (*vx_unittest_f)(int argc, char *argv[]);
typedef struct _vx_unittest_t {
    vx_status status;
    vx_char name[VX_MAX_KERNEL_NAME];
    vx_unittest_f unittest;
} vx_unittest;


/*! The array of supported unit tests */
vx_unittest unittests[] = {
    {VX_FAILURE, "mytarget 3dlut",&vx_test_mytarget_3dlut},
    {VX_FAILURE, "canny edge detection", &vx_test_edge_detection_canny},
    {VX_FAILURE, "sobel 3x3 edge detection", &vx_test_edge_detection_sobel3x3},
    {VX_FAILURE, "lib extras laplacian3x3 filter", &vx_test_lib_ext_laplacian3x3},
    {VX_FAILURE, "mylib rotation", &vx_test_mylib_rotation},
};

/*! \brief The main unit test.
 * \param argc The number of arguments.
 * \param argv The array of arguments.
 * \return vx_status
 * \retval 0 Success.
 * \retval !0 Failure of some sort.
 */
int main(int argc, char *argv[])
{
    vx_uint32 i;
    vx_uint32 passed = 0;
    vx_bool stopOnErrors = vx_false_e;

    if (argc == 2 && ((strncmp(argv[1], "-?", 2) == 0) ||
                      (strncmp(argv[1], "--list", 6) == 0) ||
                      (strncmp(argv[1], "-l", 2) == 0) ||
                      (strncmp(argv[1], "/?", 2) == 0)))
    {
        vx_uint32 t = 0;
        for (t = 0; t < dimof(unittests); t++)
        {
            printf("%u: %s\n", t, unittests[t].name);
        }
        /* we just want to know which graph is which */
        return 0;
    }
    else if (argc >= 3 && strncmp(argv[1],"-t",2) == 0)
    {
        int c = atoi(argv[2]);
        if (c < (int)dimof(unittests))
        {
            unittests[c].status = unittests[c].unittest(argc, argv);
            printf("[%u][%s] %s, error = %d\n", c, (unittests[c].status == VX_SUCCESS?"PASSED":"FAILED"), unittests[c].name, unittests[c].status);
            return unittests[c].status;
        }
        else
            return -1;
    }
    else if (argc == 2 && strncmp(argv[1],"-s",2) == 0)
    {
        stopOnErrors = vx_true_e;
    }
    for (i = 0; i < dimof(unittests); i++)
    {
        unittests[i].status = unittests[i].unittest(argc, argv);
        if (unittests[i].status == VX_SUCCESS)
        {
            printf("[PASSED][%02u] %s\n", i, unittests[i].name);
            passed++;
        }
        else
        {
            printf("[FAILED][%02u] %s, error = %d\n", i, unittests[i].name, unittests[i].status);
            if (stopOnErrors == vx_true_e)
            {
                break;
            }
        }
    }
    printf("Passed %u out of %lu\n", passed, dimof(unittests));
    if (passed == dimof(unittests))
        return 0;
    else
        return -1;
}
