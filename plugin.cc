// Copyright (c) 2011 The Native Client Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/rect.h>
#include <ppapi/cpp/size.h>
#include <ppapi/cpp/input_event.h>

#include <sys/stat.h>
#include <sys/mount.h>
#include <nacl_io/nacl_io.h>
#include <nacl_io/nacl_io.h>

#include <SDL_video.h>

extern "C" {
	extern int sdl_main(int argc, const char *argv[]);
}

#include <SDL.h>
//#include <SDL_nacl.h>

class PluginInstance : public pp::Instance {
public:
	explicit PluginInstance(PP_Instance instance, PPB_GetInterface browser_interface) :
        pp::Instance(instance),
        sdl_main_thread_(0),
        width_(0),
        height_(0),
        browser_interface_(browser_interface)
	{}

	~PluginInstance()
	{
		if (sdl_main_thread_)
		{
			pthread_join(sdl_main_thread_, NULL);
		}
	}

	virtual void DidChangeView(const pp::Rect& position, const pp::Rect& clip)
	{
		printf("did change view, new %dx%d, old %dx%d\n",
		                position.size().width(), position.size().height(),
		                width_, height_);

		if (position.size().width() == width_
		    && position.size().height() == height_)
			return;  // Size didn't change, no need to update anything.

		if (sdl_thread_started_ == false)
		{
			width_ = position.size().width();
			height_ = position.size().height();

			/*SDL_NACL_SetInstance(pp_instance(), browser_interface_, width_, height_);

			// It seems this call to SDL_Init is required. Calling from
			// sdl_main() isn't good enough.
			// Perhaps it must be called from the main thread?
			int lval = SDL_Init(SDL_INIT_VIDEO);
			assert(lval >= 0);
            if( lval < 0 )
            {
                fprintf(stderr,"SDL_Init failed\n");
                exit(1);
            }
            */

            if (0 == pthread_create(&sdl_main_thread_, NULL, sdl_thread, this)) {
				sdl_thread_started_ = true;
            }
		}
	}

	bool HandleInputEvent(const pp::InputEvent& event)
	{
		//SDL_NACL_PushEvent(event.pp_resource());
		return true;
	}

	bool Init(uint32_t argc, const char* argn[], const char* argv[])
	{
        nacl_io_init_ppapi(pp_instance(),browser_interface_);

        umount("/");
        mount("", "/", "memfs", 0, NULL);
        mkdir("/mnt", 0777);
        mkdir("/mnt/http", 0777);
        const char* data_url = getenv("NACL_DATA_URL");  // returns NULL
        if (!data_url)
            data_url = "./";
        int res = mount( data_url, "/mnt/http", "httpfs", 0,
                         "allow_cross_origin_requests=true,allow_credentials=false" );
 
        if( res != 0 )
        {
            fprintf(stderr,"HTTP mount error %d\n", res);
            exit(1);
        }
        
        return true;
	}

private:
	bool sdl_thread_started_;
	pthread_t sdl_main_thread_;
	int width_;
	int height_;
    PPB_GetInterface browser_interface_;
    
	static void* sdl_thread(void* param)
	{
		sdl_main(0, NULL);
		return NULL;
	}
};

class PepperModule : public pp::Module
{
public:
	virtual pp::Instance* CreateInstance(PP_Instance instance)
	{
		return new PluginInstance(instance, get_browser_interface());
	}
};

namespace pp
{
	Module* CreateModule()
	{
		return new PepperModule();
	}
}
