#
#  Copyright (c) 2019, The OpenThread Authors.
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

set(COMMON_INCLUDES
    ${OT_PUBLIC_INCLUDES}
    ${PROJECT_SOURCE_DIR}/openthread/src/core
    ${PROJECT_SOURCE_DIR}/openthread/examples/platforms    
    ${PROJECT_SOURCE_DIR}/src
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

if(PLAFFORM_CONFIG_ENABLE_SLEEP)
    message(STATUS "PLAFFORM_CONFIG_ENABLE_SLEEP defined: " ${PLAFFORM_CONFIG_ENABLE_SLEEP})
    add_definitions(-DPLAFFORM_CONFIG_ENABLE_SLEEP)
endif()

if(OT_FTD)
    include(ftd.cmake)
endif()

if(OT_MTD)
    include(mtd.cmake)
    include(button_mtd.cmake)
endif()
