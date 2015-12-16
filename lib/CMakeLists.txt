project(libGPCAVpp)

set(SOURCE_FILES 
  "src/CodecIDs.cpp"
  "src/Player.cpp"
  "src/Server.cpp"
  "src/Demuxer.cpp" 
  "src/Stream.cpp"
  #"src/Packet.cpp"
  "src/VideoStream.cpp"
  "src/VideoDecoder.cpp"
  "src/DecoderBase.cpp" 
  "src/Error.cpp"
  "src/Frame.cpp"
  "src/VideoFrame.cpp"
  "src/Rational.cpp"
  "src/Muxer.cpp"
  "src/opengl/YUVPainter.cpp"
  "src/HighResClock.cpp"
  "include/gpc/_av/internal/Decoder.ipp"
)

set(HEADER_FILES 
  "include/gpc/av" 
  "include/gpc/av.hpp"
  "include/gpc/_av/MediaType.hpp"
  "include/gpc/_av/CodecIDs.hpp"
  "include/gpc/_av/config.hpp"
  "include/gpc/_av/Player.hpp"
  "include/gpc/_av/Server.hpp"
  "include/gpc/_av/Demuxer.hpp"
  "include/gpc/_av/Stream.hpp"
  #"include/gpc/_av/Packet.hpp"
  "include/gpc/_av/VideoStream.hpp"
  "include/gpc/_av/internal/Decoder.hpp"
  "include/gpc/_av/internal/Decoder_Impl.hpp"
  "include/gpc/_av/VideoDecoder.hpp"
  "include/gpc/_av/Error.hpp"
  "include/gpc/_av/Frame.hpp"
  "include/gpc/_av/VideoFrame.hpp"
  "include/gpc/_av/Rational.hpp"
  "include/gpc/_av/internal/HighResClock.hpp"
  "include/gpc/_av/opengl/YUVPainter.hpp"
  "include/gpc/_av/Muxer.hpp"
  "include/gpc/_av/common.hpp"
  "include/gpc/Rational.hpp"  # TODO: move into its own library
  "src/checked_calls.hpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} BEFORE
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    #$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    #$<INSTALL_INTERFACE:include> # TODO
)

# SHADER FILES ------------------------------------------------------

set(SHADER_FILES "src/opengl/vertex.glsl" "src/opengl/fragment.glsl")

source_group("Shader files" FILES ${SHADER_FILES})

# TODO: make this into a CMake module

set(GENERATED "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_include_directories("${PROJECT_NAME}" PUBLIC ${GENERATED})

# Find the GPC Bin2C utility
find_package(GPCBin2C REQUIRED)

# Add a custom target and a dependency for each shader file

foreach(shader ${SHADER_FILES})
  get_filename_component(name "${shader}" NAME)
  set(shader_header "${GENERATED}/${name}.h")
  add_custom_command(
    OUTPUT ${shader_header}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${shader}
    COMMAND GPCBin2C --input=${CMAKE_CURRENT_SOURCE_DIR}/${shader} --output=${shader_header}
  )
  target_sources(${PROJECT_NAME} PRIVATE ${SHADER_FILES} )
  target_sources(${PROJECT_NAME} PRIVATE ${shader_header})
endforeach()

# DEPENDENCIES -------------------------------------------------------

# FFmpeg

foreach (lib "avformat" "avcodec" "avutil" "swscale")

	# Library files
	find_library(${lib}_LIB_RELEASE "${lib}")
	if (NOT ${lib}_LIB_RELEASE)
		message(FATAL_ERROR "Could not find release version of library \"${lib}\"")
	endif()
	find_library(${lib}_LIB_DEBUG "${lib}")
	if (NOT ${lib}_LIB_DEBUG)
		message(FATAL_ERROR "Could not find debug version of library \"${lib}\"")
	endif()
	target_link_libraries(${PROJECT_NAME} PUBLIC 
		$<$<CONFIG:Release>:${${lib}_LIB_RELEASE}>
		$<$<CONFIG:Debug>:${${lib}_LIB_DEBUG}>
	)

	# Header file
	find_path(${lib}_INCLUDE "lib${lib}/${lib}.h")
	if (NOT ${lib}_INCLUDE)
		message(FATAL_ERROR "Could not find header file of library \"${lib}\"")
	endif()
	target_include_directories(${PROJECT_NAME} PRIVATE ${${lib}_INCLUDE})
	
endforeach()

# The following is said to be no longer needed with C++11, which unfortunately
# CMake expressely refuses to detect as such (only individual features) are 
# detected)
target_compile_definitions(${PROJECT_NAME} PRIVATE __STDC_CONSTANT_MACROS)

# If the following does not propagate as it should, the consumer must set the "/SAFESEH:NO" setting by itself
if (MSVC)
    target_link_libraries(${PROJECT_NAME} PUBLIC "-SAFESEH:NO")
endif()

# GLBinding (OpenGL bindings)
if (TARGET GLBinding)
    target_link_libraries(${PROJECT_NAME} PRIVATE GLBinding)
else()
    find_package(GLBinding REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLBINDING_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        $<$<CONFIG:release>:${GLBINDING_LIBRARIES}>
        $<$<CONFIG:debug>:${GLBINDING_DEBUG_LIBRARIES}>)
    #target_compile_definitions(${PROJECT_NAME} PRIVATE GLBINDING_STATIC)
endif()

# GPC GL utilities
find_package(libGPCGLUtils REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE libGPCGLUtils)

# GPC GL Wrappers
# TODO: this should get pulled in automatically by GPC GL Utils, but there is a problem with CMake there
if (NOT TARGET libGPCGLWrappers)
  find_package(libGPCGLWrappers CONFIG REQUIRED)
endif()
get_target_property(libGPCGLWrappers_INCLUDE_DIRECTORIES libGPCGLWrappers INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(${PROJECT_NAME} PRIVATE "${libGPCGLWrappers_INCLUDE_DIRECTORIES}")

# EXPORT && INSTALLATION --------------------------------------------

export(TARGETS ${PROJECT_NAME}  FILE "${PROJECT_NAME}-targets.cmake")
configure_file("PROJECT-config.cmake.in" "${PROJECT_NAME}-config.cmake" @ONLY)
export(PACKAGE ${PROJECT_NAME})

# TODO: install
