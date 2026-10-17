# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_turtledraw_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED turtledraw_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(turtledraw_FOUND FALSE)
  elseif(NOT turtledraw_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(turtledraw_FOUND FALSE)
  endif()
  return()
endif()
set(_turtledraw_CONFIG_INCLUDED TRUE)

# output package information
if(NOT turtledraw_FIND_QUIETLY)
  message(STATUS "Found turtledraw: 0.0.0 (${turtledraw_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'turtledraw' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${turtledraw_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(turtledraw_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${turtledraw_DIR}/${_extra}")
endforeach()
