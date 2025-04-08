# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.18)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(server_r5)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(app PRIVATE src/neopixel_driver.c)
target_include_directories(app PRIVATE include)