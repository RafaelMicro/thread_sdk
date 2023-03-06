#
#  Copyright (c) 2021, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#


set(OT_PLATFORM_LIB "openthread-rt58x" PARENT_SCOPE)

if(NOT OT_CONFIG)
    set(OT_CONFIG "openthread-core-RT58x-config.h")
    set(OT_CONFIG ${OT_CONFIG} PARENT_SCOPE)
endif()

if(PLAFFORM_CONFIG_ENABLE_SLEEP)
    message(STATUS "PLAFFORM_CONFIG_ENABLE_SLEEP defined: " ${PLAFFORM_CONFIG_ENABLE_SLEEP})
    add_definitions(-DPLAFFORM_CONFIG_ENABLE_SLEEP)
endif()

list(APPEND OT_PLATFORM_DEFINES
    "OPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE=\"openthread-core-RT58x-config-check.h\""
)
set(OT_PLATFORM_DEFINES ${OT_PLATFORM_DEFINES} PARENT_SCOPE)

list(APPEND OT_PLATFORM_DEFINES "OPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"${OT_CONFIG}\"")

add_library(openthread-rt58x
    $<TARGET_OBJECTS:openthread-platform-utils>
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/bsp/bsp.c
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/bsp/bsp_button.c
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/bsp/bsp_console.c
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/bsp/bsp_led.c
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/bsp/bsp_uart.c
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/bsp/bsp_uart_drv.c
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/Utility/util.c
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/Utility/util_log.c
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/Utility/util_printf.c
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/Utility/util_string.c
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/Utility/util_queue.c
)

set_target_properties(
    openthread-rt58x
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

target_link_libraries(openthread-rt58x   
    PUBLIC
        -T${PROJECT_SOURCE_DIR}/src/rt58x.ld
        -Wl,--gc-sections 
        -Wl,--print-memory-usage 
        -Wl,-Map=$<TARGET_PROPERTY:NAME>.map
        ${PROJECT_SOURCE_DIR}/src/libopenthread-rt58x.a
        ${PROJECT_SOURCE_DIR}/third_party/rafael/RFB/librafael-rt58x-rfb.a
        rafael-rt58x-driver
    PRIVATE 
        ot-config
)

target_compile_definitions(openthread-rt58x
    PUBLIC
        ${OT_PLATFORM_DEFINES}
)

target_compile_options(openthread-rt58x PRIVATE
    ${OT_CFLAGS}
    -Wno-unknown-pragmas
    -Wno-sign-compare
    -Wno-unused-function
    -Wno-unused-parameter
    -Wno-empty-body
    -w
)

target_include_directories(openthread-rt58x PRIVATE
    ${OT_PUBLIC_INCLUDES}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/openthread/src/core
    ${PROJECT_SOURCE_DIR}/openthread/examples/platforms
    ${PROJECT_SOURCE_DIR}/third_party/rafael/RFB/include
    ${PROJECT_SOURCE_DIR}/third_party/rafael/RUCI/include
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/bsp/Include
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Portable/Utility/include
    ${PROJECT_SOURCE_DIR}/third_party/rafael/CMSIS/Include
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Driver/Include
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Driver/Peripheral/Include
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Driver/Crypto/include
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Driver/PHY/include
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Driver/PHY/rf_fw
    ${PROJECT_SOURCE_DIR}/third_party/rafael/Driver/PHY/rt569mp/include
)
