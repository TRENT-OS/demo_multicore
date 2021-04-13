#
# Demo Multicore
#
# Copyright (C) 2019-2020, Hensoldt Cyber GmbH
#

cmake_minimum_required(VERSION 3.7.2)

os_set_config_file(system_config.h)

#-------------------------------------------------------------------------------
project(tests_multicore C)

DeclareCAmkESComponent(
    multicore_app
    INCLUDES
        # no include paths needed
    SOURCES
        components/multicore_app/src/main.c
    C_FLAGS
        -Wall
        -Werror
    LIBS
        os_core_api
        TimeServer_client
)

TimeServer_DeclareCAmkESComponent(
    TimeServer
)

DeclareAndCreateCamkESSystem(demo_multicore.camkes)
GenerateSimulateScript()
