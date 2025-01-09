set( TARGET_NAME openvx_test )

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/lib)

#3.head file path，头文件目录
INCLUDE_DIRECTORIES(inc)
message(STATUS "VX_HEADER_DIR: ${VX_HEADER_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
include_directories( BEFORE
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${VX_HEADER_DIR}
                     )
#4.source directory，源文件目录
AUX_SOURCE_DIRECTORY(src DIR_SRCS)

find_package( OpenCV REQUIRED )
SET(LIBS ${LIBS} ${OpenCV_LIBS})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++11")
SET(LIBS ${LIBS} pthread vx_xyz_lib openvx-debug-lib openvx-extras-lib openvx-helper openvx vxu openvx-mytarget)

#6.add executable file，添加要编译的可执行文件
ADD_EXECUTABLE(${TARGET_NAME} ${DIR_SRCS})

#7.add link library，添加可执行文件所需要的库，比如我们用到了libpthread.so（命名规则：lib+name+.so），就添加该库的名称
TARGET_LINK_LIBRARIES(${TARGET_NAME} ${LIBS} )
install ( TARGETS ${TARGET_NAME} 
          RUNTIME DESTINATION bin
          ARCHIVE DESTINATION lib
          LIBRARY DESTINATION bin )