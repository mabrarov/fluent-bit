# FindLibYAML
# -----------
#
# Find the LibYAML library (https://github.com/yaml/libyaml).
#
# This module finds an installed LibYAML library and determines its version.
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following IMPORTED targets:
#
# YAML::LibYAML
#   The LibYAML library, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# LIBYAML_FOUND
#   System has the LibYAML library.
# LIBYAML_INCLUDE_DIR
#   The LibYAML include directory.
# LIBYAML_LIBRARY
#   The LibYAML library.
#
# Hints
# ^^^^^
#
# The following variables may be set to control search behavior:
#
# LIBYAML_ROOT_DIR
#   Set to the root directory of an LibYAML installation.
#
# LIBYAML_USE_STATIC_LIB
#
#   Set to ``TRUE`` to look for static libraries.

find_package(PkgConfig)
# libyaml's corresponding pkg-config file is yaml-0.1.pc.
# We should search it first.
pkg_check_modules(PC_LIBYAML QUIET yaml-0.1)

if(PC_LIBYAML_FOUND)
  set(_LIBYAML_INCLUDE_SEARCH
    PATHS
    ${PC_LIBYAML_INCLUDEDIR}
    ${PC_LIBYAML_INCLUDE_DIRS}
  )
  set(_LIBYAML_LIB_SEARCH_RELEASE
    PATHS
    ${PC_LIBYAML_LIBDIR}
    ${PC_LIBYAML_LIBRARY_DIRS}
  )
  set(_LIBYAML_LIB_SEARCH_DEBUG ${_LIBYAML_LIB_SEARCH_RELEASE})
elseif(LIBYAML_ROOT_DIR)
  set(_LIBYAML_ROOT_HINTS_AND_PATHS
    HINTS
    ${LIBYAML_ROOT_DIR}
    NO_DEFAULT_PATH
  )
  set(_LIBYAML_INCLUDE_SEARCH
    ${_LIBYAML_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES
    "include"
  )
  set(_LIBYAML_LIB_SEARCH_RELEASE
    NAMES_PER_DIR
    ${_LIBYAML_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES
    "lib"
    "lib64"
  )
  set(_LIBYAML_LIB_SEARCH_DEBUG ${_LIBYAML_LIB_SEARCH_RELEASE})
  unset(_LIBYAML_ROOT_HINTS_AND_PATHS)
else()
  set(_LIBYAML_LIB_SEARCH_RELEASE
    PATH_SUFFIXES
    "lib"
    "lib64"
  )
  set(_LIBYAML_LIB_SEARCH_DEBUG ${_LIBYAML_LIB_SEARCH_RELEASE})
endif()

if(LIBYAML_USE_STATIC_LIB)
  set(_LIBYAML_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
  if(MSVC)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
  else()
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a )
  endif()
endif()

find_path(LIBYAML_INCLUDE_DIR
  NAMES
  yaml.h
  ${_LIBYAML_INCLUDE_SEARCH}
)

if(MSVC)
  find_library(LIBYAML_LIBRARY_DEBUG
    NAMES
    yamld
    ${_LIBYAML_LIB_SEARCH_DEBUG}
  )
  find_library(LIBYAML_LIBRARY_RELEASE
    NAMES
    yaml
    ${_LIBYAML_LIB_SEARCH_RELEASE}
  )
  include(SelectLibraryConfigurations)
  select_library_configurations(LIBYAML)
  mark_as_advanced(LIBYAML_LIBRARY_DEBUG LIBYAML_LIBRARY_RELEASE)
else()
  find_library(LIBYAML_LIBRARY_RELEASE
    NAMES
    yaml
    ${_LIBYAML_LIB_SEARCH_RELEASE}
  )
  include(SelectLibraryConfigurations)
  select_library_configurations(LIBYAML)
  mark_as_advanced(LIBYAML_LIBRARY_RELEASE)
endif()

# Restore the original find library ordering
if(LIBYAML_USE_STATIC_LIB)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ${_LIBYAML_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibYAML
  REQUIRED_VARS
  LIBYAML_LIBRARY
  LIBYAML_INCLUDE_DIR
  FAIL_MESSAGE
  "Could NOT find LibYAML, try to set the path to LibYAML root folder in the system variable LIBYAML_ROOT_DIR"
)

if(LIBYAML_FOUND)
  set(_LIBYAML_TARGET_TYPE)
  if(LIBYAML_USE_STATIC_LIB)
    set(_LIBYAML_TARGET_TYPE "STATIC")
  else()
    set(_LIBYAML_TARGET_TYPE "SHARED")
  endif()
  add_library(YAML::LibYAML ${_LIBYAML_TARGET_TYPE} IMPORTED)
  unset(_LIBYAML_TARGET_TYPE)

  set_target_properties(YAML::LibYAML PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${LIBYAML_INCLUDE_DIR}")
  if(EXISTS "${LIBYAML_LIBRARY}")
    set_target_properties(YAML::LibYAML PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${LIBYAML_LIBRARY}")
  endif()

  if(EXISTS "${LIBYAML_LIBRARY_RELEASE}")
    set_property(TARGET YAML::LibYAML APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(YAML::LibYAML PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
      IMPORTED_LOCATION_RELEASE "${LIBYAML_LIBRARY_RELEASE}")
  endif()

  if(EXISTS "${LIB_EAY_LIBRARY_DEBUG}")
    set_property(TARGET YAML::LibYAML APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(YAML::LibYAML PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
      IMPORTED_LOCATION_DEBUG "${LIBYAML_LIBRARY_DEBUG}")
  endif()
endif()

mark_as_advanced(LIBYAML_INCLUDE_DIR)
