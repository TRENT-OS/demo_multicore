#
# Demo Multicore
#
# Copyright (C) 2019-2024, HENSOLDT Cyber GmbH
# 
# SPDX-License-Identifier: GPL-2.0-or-later
#
# For commercial licensing, contact: info.cyber@hensoldt.net
#

cmake_minimum_required(VERSION 3.7.2)

os_set_config_file(system_config.h)

# set(KernelMaxNumNodes 2 CACHE STRING "" FORCE)

#-------------------------------------------------------------------------------
project(tests_multicore C)

DeclareCAmkESComponent(
    multicore_app
    INCLUDES
        # no include paths needed
    SOURCES
        components/multicore_app/main.c
    C_FLAGS
        -Wall
        -Werror
    LIBS
        os_core_api
        lib_debug
        TimeServer_client
)

DeclareCAmkESComponent(
    Server
    SOURCES
        components/server/server.c
    LIBS
        lib_debug
)

TimeServer_DeclareCAmkESComponent(
    TimeServer
)

DeclareAndCreateCamkESSystem(demo_multicore.camkes)
GenerateSimulateScript()
