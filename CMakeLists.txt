# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# ========================================================================
# CMakeLists.txt
# ========================================================================
# A build system for our project

cmake_minimum_required(VERSION 3.13)

# ------------------------------------------------------------------------
# Compiler Setup
# ------------------------------------------------------------------------

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------
# Initialise pico_sdk from installed location
# ------------------------------------------------------------------------
# DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code
# Extension to work

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico CACHE STRING "Board type")

# ------------------------------------------------------------------------
# Include the Pico SDK
# ------------------------------------------------------------------------

include(cmake/pico_sdk_import.cmake)

# ------------------------------------------------------------------------
# Project Setup
# ------------------------------------------------------------------------

project(
  chimes_leds
  VERSION 1.0
  DESCRIPTION "Our software for controlling McGraw Tower's clock face LEDs"
  LANGUAGES C CXX
)
pico_sdk_init()
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_compile_options(-O3 -Wall -Wextra -Wno-unused-parameter)

set(PICO_LIBS
  hardware_pio
  hardware_pwm
  hardware_adc
  hardware_i2c
  pico_stdlib
  pico_cyw43_arch_none
)

#-------------------------------------------------------------------------
# get_leaf
#-------------------------------------------------------------------------
# Gets the leaf file of a path.

function(get_leaf VARNAME FILE_NAME)
  string(REGEX REPLACE "[^/]*/" "" NAME_WITHOUT_EXT ${FILE_NAME})
  set(${VARNAME} "${NAME_WITHOUT_EXT}" PARENT_SCOPE)
endfunction()

#------------------------------------------------------------------------
# remove_extension
#------------------------------------------------------------------------
# A function to remove a file's extension

function(remove_extension VARNAME FILE_NAME)
  string(REGEX REPLACE "\\.[^.]*$" "" NAME_WITHOUT_EXT ${FILE_NAME})
  set(${VARNAME} "${NAME_WITHOUT_EXT}" PARENT_SCOPE)
endfunction()

#------------------------------------------------------------------------
# add_prefix
#------------------------------------------------------------------------
# A function to add a prefix to all elements of a list

function(add_prefix LIST_VAR PREFIX)
  set(NEW_LIST_VAR "")
  foreach(ITEM ${${LIST_VAR}})
    list(APPEND NEW_LIST_VAR "${PREFIX}${ITEM}")
  endforeach(ITEM)
  set(${LIST_VAR} ${NEW_LIST_VAR} PARENT_SCOPE)
endfunction()

# ------------------------------------------------------------------------
# Compile subdirectories as libraries
# ------------------------------------------------------------------------

set(SUBDIRS
  can
  ui
  interface
  utils
  wifi
  sensors
)

set(GLOBAL_SRC_FILES "")
set(GLOBAL_PIO_FILES "")

foreach(SUBDIR ${SUBDIRS})
  unset(SRC_FILES)
  unset(PIO_FILES)

  add_subdirectory(${SUBDIR})
  add_prefix(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/)
  add_prefix(PIO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/)
  set(GLOBAL_SRC_FILES ${GLOBAL_SRC_FILES} ${SRC_FILES})
  set(GLOBAL_PIO_FILES ${GLOBAL_PIO_FILES} ${PIO_FILES})
endforeach(SUBDIR)

# ------------------------------------------------------------------------
# Add main executable
# ------------------------------------------------------------------------

unset(APP_FILES)
unset(PIO_FILES)

add_subdirectory(apps)
add_prefix(APP_FILES "apps/")
add_prefix(PIO_FILES "apps/")

foreach(APP_FILE_PATH ${APP_FILES})
  get_leaf(APP_FILE ${APP_FILE_PATH})
  remove_extension(APP_BIN ${APP_FILE})
  add_executable(${APP_BIN} ${APP_FILE_PATH} ${GLOBAL_SRC_FILES})
  target_link_libraries(${APP_BIN} ${PICO_LIBS})

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${APP_BIN})

  # Generate PIO headers
  foreach(PIO_FILE ${PIO_FILES} ${GLOBAL_PIO_FILES})
    pico_generate_pio_header(${APP_BIN} ${PIO_FILE})
  endforeach(PIO_FILE)
endforeach(APP_FILE_PATH)
