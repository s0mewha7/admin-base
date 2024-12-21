# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/admin-base_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/admin-base_autogen.dir/ParseCache.txt"
  "admin-base_autogen"
  )
endif()
