// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file gedanken.c
	\brief Utility to use memory mapped files as virtual heap.

	^^
*/

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h> // dlsym
#endif

#include <tempora/all.h>
#include <kaji/gedanken.h>
#include <kaji/all.h>

#ifdef USE_GEDANKEN
static kaji_t* g_kaji = NULL;
static int g_use_kaji = 0;
static uint8_t g_system_functions_initialized = 0;

static void* (*system_malloc)(size_t) = NULL;
static void* (*system_calloc)(size_t, size_t) = NULL;
static void* (*system_realloc)(void*, size_t) = NULL;
static void* (*system_free)(void*) = NULL;

void
__gedanken_initialize_memory_functions() {
	if (0 == g_system_functions_initialized) {
		#if defined(_WIN32)
		HMODULE hModule = GetModuleHandle(TEXT(UCRTBASEDLL_NAME));
		assert(NULL != hModule && "Could not get module :/");
		system_malloc = GetProcAddress(hModule, 
			TEXT("malloc")
		);
		system_calloc = GetProcAddress(hModule, 
			TEXT("calloc")
		);
		system_realloc = GetProcAddress(hModule, 
			TEXT("realloc")
		);
		system_free = GetProcAddress(hModule, 
			TEXT("free")
		);
		#else
		system_malloc = dlsym(RTLD_NEXT, "malloc");
		system_calloc = dlsym(RTLD_NEXT, "calloc");
		system_realloc = dlsym(RTLD_NEXT, "realloc");
		system_free = dlsym(RTLD_NEXT, "free");
		#endif
		g_system_functions_initialized = 1;
	}
}

void* malloc(size_t size) {
    return gedanken_malloc(size);
}

void* calloc(size_t number_of_elements, size_t element_size) {
    return gedanken_calloc(number_of_elements, element_size);
}

void* realloc(void* data, size_t size) {
    return gedanken_realloc(data, size);
}

void free(void* memory) {
    gedanken_free(memory);
}

void* gedanken_malloc(size_t size) {
	__gedanken_initialize_memory_functions();

	if (gedanken_is_activated()) {
		KAJI_LOG("Allocating memory (malloc) using kaji ...\n");
		return kaji_allocate(g_kaji, size);
	}

	KAJI_LOG("Allocating memory (malloc) using SYSTEM ...\n");
	return system_malloc(size);
}

void* gedanken_calloc(size_t number_of_elements, size_t element_size) {
	__gedanken_initialize_memory_functions();

	if (gedanken_is_activated()) {
		KAJI_LOG("Allocating memory (calloc) using kaji ...\n");
		size_t total_size = number_of_elements * element_size;
		void* data = kaji_allocate(g_kaji, total_size);
		memset(data, 0, total_size);
		return data;
	}

	KAJI_LOG("Allocating memory (calloc) using SYSTEM ...\n");
	return system_calloc(number_of_elements, element_size);
}

void* gedanken_realloc(void* data, size_t size) {
	__gedanken_initialize_memory_functions();

	// The realloc() function shall change the size of the memory object pointed
	// to by ptr to the size specified by size. The contents of the object shall
	// remain unchanged up to the lesser of the new and old sizes. If the new size
	// of the memory object would require movement of the object, the space for
	// the previous instantiation of the object is freed. If the new size is larger,
	// the contents of the newly allocated portion of the object are unspecified.
	// If size is 0 and ptr is not a null pointer, the object pointed to is freed.
	// If the space cannot be allocated, the object shall remain unchanged.

	// If ptr is a null pointer, realloc() shall be equivalent to malloc() for
	// the specified size.

	// If ptr does not match a pointer returned earlier by calloc(), malloc(),
	// or realloc() or if the space has previously been deallocated by a call
	// to free() or realloc(), the behavior is undefined.

	// The order and contiguity of storage allocated by successive calls to
	// realloc() is unspecified. The pointer returned if the allocation
	// succeeds shall be suitably aligned so that it may be assigned to a
	// pointer to any type of object and then used to access such an object in
	// the space allocated (until the space is explicitly freed or reallocated).
	// Each such allocation shall yield a pointer to an object disjoint from any
	// other object. The pointer returned shall point to the start
	// (lowest byte address) of the allocated space.
	// If the space cannot be allocated, a null pointer shall be returned.

	// RETURN VALUE
	// Upon successful completion with a size not equal to 0, realloc() shall
	// return a pointer to the (possibly moved) allocated space. If size is 0,
	// either a null pointer or a unique pointer that can be successfully
	// passed to free() shall be returned. If there is not enough available memory,
	// realloc() shall return a null pointer [CX] [Option Start] and set errno to [ENOMEM].

	if (gedanken_is_activated()) {
		KAJI_LOG("Allocating memory (realloc) using kaji ...\n");

		kaji_fragment_t f;
		if (kaji_fragment_find(g_kaji, &f, data)) {
			return NULL;
		}
		KAJI_LOG("Preparing swap space of %llu bytes in separate memory ...\n", f.size);
		// TODO: only allocate swap space that's needed when shrinking.
		void* swap_space = system_malloc(f.size);
		memcpy(swap_space, data, f.size);
		//memset(data, 0, f.size);
		kaji_free(g_kaji, data);

		KAJI_LOG("Allocating new space through kaji ...\n");
		void* new_space = kaji_allocate(g_kaji, size);
		if (NULL != new_space) {
			if (size > f.size) {
				KAJI_LOG("Expanding allocation ...\n");
				memset(new_space, 0, size);
				memcpy(new_space, swap_space, f.size);
			}
			else {
				KAJI_LOG("Shrinking allocation ...\n");
				memcpy(new_space, swap_space, size);
			}
		}
		else {
			KAJI_LOG("Could not allocate space through kaji ...\n");
		}
		system_free(swap_space);

		return new_space;
	}
	else {
		KAJI_LOG("Freeing memory using SYSTEM ...\n");
		return system_realloc(data, size);
	}
}

void gedanken_free(void* memory) {
	__gedanken_initialize_memory_functions();

	if (gedanken_is_activated()) {
		KAJI_LOG("Freeing memory using kaji ...\n");
		kaji_free(g_kaji, memory);
	}
	else {
		KAJI_LOG("Freeing memory using SYSTEM ...\n");
		system_free(memory);
	}
}

int gedanken_initialize(uint64_t head_size, const char* filename) {
	KAJI_LOG("Fetching system memory functions ...\n");
	__gedanken_initialize_memory_functions();

	char tmppath[L_tmpnam];
	if (TEMPORA_ERROR == tempora_read(tmppath, L_tmpnam)) {
		KAJI_LOG(":/\n");
		return -1;
	}

	size_t l = strlen(tmppath);
	if (tmppath[l-1] != '/') {
		strcat(tmppath, "/");
	}
	strcat(tmppath, filename);

	g_kaji = kaji_materialize();
	if (NULL == g_kaji) {
		KAJI_LOG("Could not materialize kaji ...\n");
		return 1;
	}
	KAJI_LOG("Binding ...\n");
	while (0 != kaji_bind(g_kaji, tmppath, head_size)) {
		KAJI_LOG("Error binding :/ (errno: %i, %s)\n"
			, errno, strerror(errno));
		switch(errno) {
			case ENOMEM:
				KAJI_LOG("Resizing / filling ...\n");
				kaji_file_expand(tmppath, head_size);
			break;
			case ENOENT:
				KAJI_LOG("Creating new temp file ...\n");
				kaji_file_create(tmppath, head_size);
			break;
			default:
				return 13;
		}
	}

	KAJI_LOG("Activating kaji ...\n");
	g_use_kaji = 1;

	return 0;
}

kaji_t* gedanken_kaji() {
	return g_kaji;
}

uint8_t gedanken_is_activated() {
	return g_use_kaji;
}

void gedanken_activate(uint8_t active) {
	g_use_kaji = active;
}

void gedanken_shutdown() {
	g_use_kaji = 0;

	KAJI_LOG("Shutting down gedanken ...\n");
	if (0 != kaji_release(g_kaji)) {
		KAJI_LOG("Error releasing kaji :/ (errno: %i, %s)\n"
			, errno, strerror(errno));
	}

	KAJI_LOG("Dematerializing ...\n");
	kaji_dematerialize(g_kaji);

	g_kaji = NULL;
}
#else
#if defined(WIN32)
#pragma message( "Skipping definition of gedanken API!" )
#else
#warning "Disabling gedanken. Using stubs for API calls!"
#endif
#endif