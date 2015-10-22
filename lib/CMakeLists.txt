project("libGPC-AVpp")

set(SOURCE_FILES "src/av.cpp")

set(HEADER_FILES 
  "include/gpc/av" 
  "include/gpc/av.hpp" 
)

add_library("libGPC-AVpp" STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories("${PROJECT_NAME}" BEFORE
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    #$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    #$<INSTALL_INTERFACE:include> # TODO
)

# EXPORT && INSTALLATION --------------------------------------------

export(TARGETS "${PROJECT_NAME}"  FILE "${PROJECT_NAME}-targets.cmake")
configure_file("${PROJECT_NAME}-config.cmake.in" "${PROJECT_NAME}-config.cmake" @ONLY)
export(PACKAGE "${PROJECT_NAME}")

# TODO: install
