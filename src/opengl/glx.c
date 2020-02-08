/*
Copyright (C) 2016-2017 Björn Spindel

This file is part of libstrangle.

libstrangle is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libstrangle is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libstrangle.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _GNU_SOURCE

#include "glx.h"
#include "gl.h"
#include "libstrangle.h"
#include "real_dlsym.h"
#include "limiter.h"
#include "config.h"

#include <dlfcn.h>
#include <stdlib.h>

void* strangle_requireGlxFunction( const char* name ) {
	static void *(*real_glXGetProcAddress)( const unsigned char* );
	static void *(*real_glXGetProcAddressARB)( const unsigned char* );
	if (real_glXGetProcAddress == NULL) {
		real_glXGetProcAddress = strangle_requireFunction( "glXGetProcAddress" );
	}
	if (real_glXGetProcAddressARB == NULL) {
		real_glXGetProcAddress = strangle_requireFunction( "glXGetProcAddressARB" );
	}

	void (*func)()
	= real_glXGetProcAddress( (const unsigned char*) name );

	if (func == NULL) {
		func = real_glXGetProcAddressARB( (const unsigned char*) name );
	}

	if (func == NULL) {
		func = strangle_requireFunction( name );
	}

	return func;
}

EXPORT
void glXSwapBuffers( void* dpy, void* drawable ) {
	static void (*realFunction)( void*, void* );
	if (realFunction == NULL) {
		realFunction = strangle_requireGlxFunction( __func__ );
	}

	StrangleConfig *config = getConfig();

	// There is probably a better place for this but where???
	if ( config->mipLodBias ) {
		void (*glTexEnvf)( int, int, float ) = strangle_requireFunction("glTexEnvf");
		glTexEnvf( GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, *config->mipLodBias );
	}

	if ( config->glfinish != NULL && *config->glfinish == true ) {
		glFinish();
	}

	// The buffer swap is called before the wait in hope that it will reduce perceived input lag
	realFunction( dpy, drawable );
	limiter( config->targetFrameTime );
}

EXPORT
void glXSwapIntervalEXT( void* dpy, void* drawable, int interval ) {
	static void (*realFunction)( void*, void*, int );
	if (realFunction == NULL) {
		realFunction = strangle_requireGlxFunction( __func__ );
	}

	realFunction( dpy, drawable, getInterval( interval ) );
}

EXPORT
int glXSwapIntervalSGI( int interval ) {
	static int (*realFunction)( int );
	if (realFunction == NULL) {
		realFunction = strangle_requireGlxFunction( __func__ );
	}

	return realFunction( getInterval( interval ) );
}

EXPORT
int glXSwapIntervalMESA( unsigned int interval ) {
	static int (*realFunction)( int );
	if (realFunction == NULL) {
		realFunction = strangle_requireGlxFunction( __func__ );
	}

	return realFunction( getInterval( interval ) );
}

EXPORT
void* glXGetProcAddress( const unsigned char* procName ) {
	static void* (*realFunction)( const unsigned char* );
	if (realFunction == NULL) {
		realFunction = strangle_requireFunction( __func__ );
	}

	void* func = getStrangleFunc( (const char*)procName );
	if ( func != NULL ) {
		return func;
	}

	return realFunction( procName );
}

EXPORT
void* glXGetProcAddressARB( const unsigned char* procName ) {
	static void *(*realFunction)( const unsigned char* );
	if (realFunction == NULL) {
		realFunction = strangle_requireFunction( __func__ );
	}

	void* func = getStrangleFunc( (const char*)procName );
	if ( func != NULL ) {
		return func;
	}

	return realFunction( procName );
}

EXPORT
bool glXMakeCurrent( void* dpy, void* drawable, void* ctx ) {
	static bool (*realFunction)( void*, void*, void* );
	if (realFunction == NULL) {
		realFunction = strangle_requireGlxFunction( __func__ );
	}

	bool ret = realFunction( dpy, drawable, ctx );
	setVsync();
	return ret;
}