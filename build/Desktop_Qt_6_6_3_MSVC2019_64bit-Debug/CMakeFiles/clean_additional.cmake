# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "src\\core\\CMakeFiles\\core_autogen.dir\\AutogenUsed.txt"
  "src\\core\\CMakeFiles\\core_autogen.dir\\ParseCache.txt"
  "src\\core\\core_autogen"
  )
endif()
