project("libGPC-AVpp")

set(SOURCE_FILES 
  "src/av.cpp"
  "src/player.cpp"
  "src/Demuxer.cpp" 
  "src/VideoDecoder.cpp"
  "src/DecoderBase.cpp" 
  "src/Error.cpp"
  "src/Frame.cpp"
)

set(HEADER_FILES 
  "include/gpc/av" 
  "include/gpc/av.hpp"
  "include/gpc/_av/config.hpp"
  "include/gpc/_av/Player.hpp"
  "include/gpc/_av/Demuxer.hpp"
  "include/gpc/_av/internal/DecoderBase.hpp"
  "include/gpc/_av/internal/DecoderBase_Impl.hpp"
  "include/gpc/_av/VideoDecoder.hpp"
  "include/gpc/_av/Error.hpp"
  "include/gpc/_av/Frame.hpp"
  "src/checked_calls.hpp"
)

add_library("libGPC-AVpp" STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories("${PROJECT_NAME}" BEFORE
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    #$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    #$<INSTALL_INTERFACE:include> # TODO
)

# DEPENDENCIES -------------------------------------------------------

find_library(avformat_LIB avformat)
find_path(avformat_INCLUDE libavformat/avformat.h)
find_library(avcodec_LIB avcodec)
find_path(avcodec_INCLUDE libavcodec/avcodec.h)
find_library(avutil_LIB avutil)
find_path(avutil_INCLUDE libavutil/avutil.h)
find_library(swscale_LIB swscale)
find_path(swscale_INCLUDE libswscale/swscale.h)

target_link_libraries(${PROJECT_NAME} PUBLIC ${avformat_LIB} ${avcodec_LIB} ${avutil_LIB} ${swscale_LIB})
target_include_directories(${PROJECT_NAME} PRIVATE ${avformat_INCLUDE} ${avcodec_INCLUDE} ${avutil_INCLUDE} ${swscale_INCLUDE})
target_compile_definitions(${PROJECT_NAME} PRIVATE __STDC_CONSTANT_MACROS)

# The following does not work: the dependee must set the /SAFESEH:NO linker flag itself.
if (MSVC)
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
endif()

# EXPORT && INSTALLATION --------------------------------------------

export(TARGETS "libGPC-AVpp"  FILE "${PROJECT_NAME}-targets.cmake")
configure_file("libGPC-AVpp-config.cmake.in" "libGPC-AVpp-config.cmake" @ONLY)
export(PACKAGE "libGPC-AVpp")

# TODO: install
