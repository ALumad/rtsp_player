cmake_minimum_required(VERSION 3.5)

project(rtsp-player LANGUAGES CXX)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GSTLIBS REQUIRED
        gobject-2.0
        glib-2.0)

pkg_search_module(GLIB REQUIRED glib-2.0) 
pkg_check_modules(GST REQUIRED 
        gstreamer-plugins-base-1.0 
        gstreamer-base-1.0 
        gstreamer-1.0 
        gstreamer-rtsp-1.0)

include_directories(${GST_INCLUDE_DIRS})
link_directories(${GST_LIBRARIES})

add_executable(rtsp_player
        main.cpp 
        gstwrap/client.cpp
        gstwrap/pipeline.cpp
        gstwrap/gstcallback.cpp)


target_link_libraries(rtsp_player ${GST_LIBRARIES})


file(COPY 
        ${CMAKE_CURRENT_SOURCE_DIR}/rtsp.conf
     DESTINATION ${CMAKE_BINARY_DIR}
)

file(COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/cert
        DESTINATION ${CMAKE_BINARY_DIR}
)
