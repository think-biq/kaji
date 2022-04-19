// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file gedanken.c
	\brief Utility to use memory mapped files as virtual heap.

	^^
*/

#include <errno.h>
#include <assert.h>
#include <dlfcn.h> // dlsym
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tempora/all.h>
#include <kaji/gedanken.h>
#include <kaji/all.h>

#ifdef USE_GEDANKEN
static kaji_t* g_kaji = NULL;
static int g_use_kaji = 0;
static uint8_t g_system_functions_initialized = 0;

static void* (*system_malloc)(size_t) = NULL;
static void* (*system_free)(void*) = NULL;

void
__gedanken_initialize_memory_functions() {
	if (0 == g_system_functions_initialized) {
		system_malloc = dlsym(RTLD_NEXT, "malloc");
		system_free = dlsym(RTLD_NEXT, "free");
		g_system_functions_initialized = 1;
	}
}

void* malloc(size_t size) {
	__gedanken_initialize_memory_functions();

	if (gedanken_is_activated()) {
		printf("Allocating memory (malloc) using kaji ...\n");
		return kaji_allocate(g_kaji, size);
	}

	printf("Allocating memory (malloc) using SYSTEM ...\n");
    return system_malloc(size);
}

void free(void* memory) {
	__gedanken_initialize_memory_functions();

	if (gedanken_is_activated()) {
		printf("Freeing memory using kaji ...\n");
		kaji_free(g_kaji, memory);
	}
	else {
		printf("Freeing memory using SYSTEM ...\n");
		system_free(memory);
	}
}

int gedanken_initialize(uint64_t head_size, const char* filename) {
	KAJI_LOG("Fetching system memory functions ...\n");
    system_malloc = dlsym(RTLD_NEXT, "malloc");
    system_free = dlsym(RTLD_NEXT, "free");

    char tmppath[L_tmpnam];
	if (TEMPORA_ERROR == tempora_read(tmppath, L_tmpnam)) {
		printf(":/\n");
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

	printf("Activating kaji ...\n");
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

	printf("Shutting down gedanken ...\n");
	if (0 != kaji_release(g_kaji)) {
		fprintf(stderr, "Error releasing kaji :/ (errno: %i, %s)\n"
			, errno, strerror(errno));
	}

	printf("Dematerializing ...\n");
	kaji_dematerialize(g_kaji);

	g_kaji = NULL;
}
#else
#warning "Skipping definition of gedanken API!"
#endif