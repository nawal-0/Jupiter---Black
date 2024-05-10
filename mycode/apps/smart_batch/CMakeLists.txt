# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(smart_badge)

target_sources(app PRIVATE src/main.c  ../../mylib/ble_receive.c ../../mylib/motion_detect.c  ../../mylib/uart.c ../../mylib/sensors/lis2dh.c ../../mylib/speaker.c)
include_directories(app PRIVATE ~/csse4011/project/mycode/include)

# if(IBEACON_RSSI)
#   zephyr_compile_definitions(IBEACON_RSSI=${IBEACON_RSSI})
# endif()