# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "src\\core\\CMakeFiles\\core_autogen.dir\\AutogenUsed.txt"
  "src\\core\\CMakeFiles\\core_autogen.dir\\ParseCache.txt"
  "src\\core\\core_autogen"
  "src\\dispatcher\\CMakeFiles\\dispatcher_autogen.dir\\AutogenUsed.txt"
  "src\\dispatcher\\CMakeFiles\\dispatcher_autogen.dir\\ParseCache.txt"
  "src\\dispatcher\\dispatcher_autogen"
  "src\\main_app\\CMakeFiles\\main_app_autogen.dir\\AutogenUsed.txt"
  "src\\main_app\\CMakeFiles\\main_app_autogen.dir\\ParseCache.txt"
  "src\\main_app\\main_app_autogen"
  "src\\network\\CMakeFiles\\network_autogen.dir\\AutogenUsed.txt"
  "src\\network\\CMakeFiles\\network_autogen.dir\\ParseCache.txt"
  "src\\network\\network_autogen"
  "src\\ui\\CMakeFiles\\ui_autogen.dir\\AutogenUsed.txt"
  "src\\ui\\CMakeFiles\\ui_autogen.dir\\ParseCache.txt"
  "src\\ui\\ui_autogen"
  "src\\user_session\\CMakeFiles\\user_session_autogen.dir\\AutogenUsed.txt"
  "src\\user_session\\CMakeFiles\\user_session_autogen.dir\\ParseCache.txt"
  "src\\user_session\\user_session_autogen"
  "src\\word_function\\CMakeFiles\\word_function_autogen.dir\\AutogenUsed.txt"
  "src\\word_function\\CMakeFiles\\word_function_autogen.dir\\ParseCache.txt"
  "src\\word_function\\word_function_autogen"
  )
endif()
