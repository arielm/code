cmake_minimum_required (VERSION 3.2.2)

project(DrawingLotsOf3dText)

set(INCLUDE_DIRS
  $ENV{CODE_PATH}
)

set(SRC_FILES
  "src/Sketch.cpp"
  "$ENV{CODE_PATH}/common/xf/Font.cpp"
  "$ENV{CODE_PATH}/common/xf/FontManager.cpp"
  "$ENV{CODE_PATH}/common/xf/FontSequence.cpp"
  "$ENV{CODE_PATH}/common/quad/QuadBatch.cpp"
)

#
# For Mojave and up
#
add_compile_definitions(
  GL_SILENCE_DEPRECATION
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
