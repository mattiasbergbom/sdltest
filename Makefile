# Copyright (c) 2012 The Native Client Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

#
# GNU Make based build file.  For details on GNU Make see:
# http://www.gnu.org/software/make/manual/make.html
#

#
# Project information
#
# These variables store project specific settings for the project name
# build flags, files to copy or install. In the examples it is typically
# only the list of sources and project name that will actually change and
# the rest of the makefile is boilerplate for defining build rules.
#
PROJECT:=sdltest
CXX_SOURCES:=plugin.cc
C_SOURCES:=sdltest.c ffmpeg_test.c libav_cmdutils.c

SOURCES:=$(CXX_SOURCES) $(C_SOURCES)
VALID_TOOLCHAINS := pnacl newlib glibc pnacl

TARGET = sdltest
DEPS = nacl_io
LIBS = 

include $(NACL_SDK_ROOT)/tools/common.mk

#
# Get pepper directory for toolchain and includes.
#
# If NACL_SDK_ROOT is not set, then assume it can be found a two directories up,
# from the default example directory location.
#
THIS_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
NACL_SDK_ROOT?=$(abspath $(dir $(THIS_MAKEFILE))../..)

#
# Compute tool paths
#
#
OSNAME:=$(shell python $(NACL_SDK_ROOT)/tools/getos.py)
TC:=pnacl
ARCH:=
TC_PATH:=$(abspath $(NACL_SDK_ROOT)/toolchain/$(OSNAME)_$(TC))

# Project Build flags
WARNINGS:=-Wno-long-long -Wall -Wswitch-enum -pedantic -Werror
SDL_CONFIG:=$(TC_PATH)/usr/bin/sdl-config
CFLAGS:=-pthread $(WARNINGS) `$(SDL_CONFIG) --cflags`
#-I/media/Extra/src/
CXXFLAGS:=-pthread -std=gnu++98 $(WARNINGS) `$(SDL_CONFIG) --cflags`
LDFLAGS:=-L/media/Extra/src/nacl_sdk/pepper_37/toolchain/linux_pnacl/usr/lib -Wl,--start-group -lSDLmain -lSDL -lppapi_cpp -lppapi -ltar -lpthread -lnacl_io -lc++ -lm -lavcodec -lavformat -lswscale -lz -lavutil -lavresample -lavdevice -lavfilter -lavcodec -Wl,--end-group 

$(warning X86_32_CXXFLAGS: ${X86_32_CXXFLAGS})
$(warning X86_64_CXXFLAGS: ${X86_64_CXXFLAGS})
$(warning ARM_CXXFLAGS: ${ARM_CXXFLAGS})

# Build rules generated by macros from common.mk:

$(foreach dep,$(DEPS),$(eval $(call DEPEND_RULE,$(dep))))
$(foreach src,$(SOURCES),$(eval $(call COMPILE_RULE,$(src),$(CFLAGS))))

# The PNaCl workflow uses both an unstripped and finalized/stripped binary.
# On NaCl, only produce a stripped binary for Release configs (not Debug).
ifneq (,$(or $(findstring pnacl,$(TOOLCHAIN)),$(findstring Release,$(CONFIG))))
$(eval $(call LINK_RULE,$(TARGET)_unstripped,$(SOURCES),$(LIBS),$(DEPS),$(LDFLAGS)))
$(eval $(call STRIP_RULE,$(TARGET),$(TARGET)_unstripped))
else
$(eval $(call LINK_RULE,$(TARGET),$(SOURCES),$(LIBS),$(DEPS)))
endif

$(eval $(call NMF_RULE,$(TARGET),))

index.html:sdltest.html
	@cp -fv $^ $@