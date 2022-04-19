// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file kaji.c
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <dlfcn.h> // dlsym

#include <spirits/all.h>

#include <kaji/all.h>

#ifdef _WIN32
#define KAJI_PATH_SIZE 256
#else
#define KAJI_PATH_SIZE 4096
#endif

#define KAJI_PAGE_SIZE sysconf(_SC_PAGESIZE)

struct kaji {
	uint8_t* memory;
	char path[KAJI_PATH_SIZE];
	uint64_t size;
	spirits_t spirits;
};
typedef
	struct kaji
	kaji_t
	;

static uint8_t g_system_functions_initialized = 0;
static void* (*system_malloc)(size_t) = NULL;
static void* (*system_free)(void*) = NULL;

void*
__kaji_malloc(size_t size) {
	assert(NULL != system_malloc && "Missing memory functions!");
	return system_malloc(size);
}

void
__kaji_free(void* memory) {
	assert(NULL != system_free && "Missing memory functions!");
	system_free(memory);
}

void
__kaji_initialize_memory_functions() {
	if (0 == g_system_functions_initialized) {
		system_malloc = dlsym(RTLD_NEXT, "malloc");
		system_free = dlsym(RTLD_NEXT, "free");
		g_system_functions_initialized = 1;
	}
}

void
kaji_print_spirits(kaji_t* ctx) {
	if (NULL == ctx) {
		printf("No spirits materialized.\n");
		return;
	}
	spirits_print_all(&(ctx->spirits));
}

kaji_t*
kaji_materialize() {
	__kaji_initialize_memory_functions();

	kaji_t* kaji = __kaji_malloc(sizeof(kaji_t));
	spirits_summon(&(kaji->spirits), 0);

	return kaji;
}

void
kaji_dematerialize(kaji_t* ctx) {
	if (NULL == ctx) return;
	__kaji_free(ctx);
}

const char*
kaji_path(kaji_t* ctx) {
	if (NULL == ctx) {
		errno = EINVAL;
		return NULL;
	}

	return '\0' == ctx->path[0]
		? NULL
		: ctx->path
		;
}

uint8_t
kaji_bind(kaji_t* ctx, const char* path, uint64_t size) {
	if (NULL == ctx) {
		errno = EINVAL;
		return 1;
	}

	if (NULL == path) {
		errno = EINVAL;
		return 2;
	}

	spirits_banish(&(ctx->spirits));

	ctx->path[0] = '\0';
	if (0 != strncmp(ctx->path, path, KAJI_PATH_SIZE)) {
		strncpy(ctx->path, path, KAJI_PATH_SIZE);
	}

	int32_t file_descriptor = open(path, O_RDWR | O_CREAT, 0666);
	if (0 > file_descriptor) {
		errno = ENOENT;
		return 3;
	}

	struct stat fs;
	stat(ctx->path, &fs);
	if (fs.st_size < size) {
		close(file_descriptor);
		errno = ENOMEM;
		return 4;
	}

	ctx->size = 0;
	ctx->memory = mmap(NULL
		, size
		, PROT_READ | PROT_WRITE
		, MAP_SHARED
		, file_descriptor
		, 0
	);
	close(file_descriptor);
	if(MAP_FAILED == ctx->memory) {
		ctx->memory = NULL;
		return 5;
	}
	ctx->size = size;

	ctx->spirits.offset = 0;
	ctx->spirits.size = ctx->size;
	ctx->spirits.available = 1;
	ctx->spirits.next = NULL;
	ctx->spirits.prev = NULL;

	return 0;
}

uint8_t
kaji_release(kaji_t* ctx) {
	if (NULL == ctx) {
		errno = EINVAL;
		return 1;
	}

	int err = munmap(ctx->memory, ctx->size);
	ctx->memory = NULL;
	ctx->size = 0;
	ctx->path[0] = '\0';
	spirits_banish(&(ctx->spirits));

	return err;
}

void
kaji_blank(kaji_t* ctx) {
	kaji_zero(ctx->path, ctx->size);
}

uint8_t
kaji_zero(const char* path, uint64_t size) {
	static const uint64_t default_data = 0;
	static const uint64_t chunk_size = sizeof default_data;

	int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (0 > fd) {
		return 3;
	}

	uint64_t cycles = size / chunk_size;
	uint64_t rest = cycles % chunk_size;
	for (uint64_t i = 0; i < cycles; ++i) {
		write(fd, &default_data, sizeof default_data);
	}
	write(fd, &default_data, rest);

	close(fd);

	return 0;
}

void*
kaji_allocate(kaji_t* ctx, uint64_t size) {
	uint64_t offset = 0;
	if (0 == spirits_allocate(&(ctx->spirits), &offset, size)) {
		printf("Allocated %llu bytes of memory at offset: %llu\n", size, offset);
	}
	else {
		printf("Allocation error :/\n");
		return NULL;
	}

	kaji_fragment_t f = {
		.offset = offset,
		.size = size
	};
	if (NULL == kaji_fragment_marshall(ctx, &f)) {
		fprintf(stderr, "Could not marhal second fragment :/\n");
		return NULL;
	}
	printf("Allocated fragment of size %llu at %llu.\n", size, offset);

	return f.data;
}

void
kaji_free(kaji_t* ctx, void* data) {
	kaji_fragment_t f;
	if (0 == kaji_fragment_find(ctx, &f, data)) {
		spirits_free(&(ctx->spirits), f.offset);
	}
	else {
		printf("Could not find fragment for data :/\n");
	}
}

void*
kaji_marshall(kaji_t* ctx, uint64_t offset, uint64_t size) {
	if (NULL == ctx || NULL == ctx->memory) {
		fprintf(stderr, "Context invalid!\n");
		return NULL;
	}

	if (ctx->size < (offset + size)) {
		fprintf(stderr, "Memory range outside of mapped file bounds!\n");
		return NULL;
	}

	return (void*)(ctx->memory + offset);
}

uint64_t
kaji_fragment_page(const kaji_fragment_t* f) {
	return (uint64_t)floor(
		f->offset / (uint64_t)KAJI_PAGE_SIZE
	);
}

uint8_t
kaji_fragment_find(kaji_t* ctx, kaji_fragment_t* f, const void* data) {
	spirits_t* next = &(ctx->spirits);
	while (NULL != next) {
		void* spirit_address = ctx->memory + next->offset;
		if (data == spirit_address) {
			f->size = next->size;
			f->offset = next->offset;
			f->data = spirit_address;

			return 0;
		}

		next = next->next;
	}

	return 1;
}

void*
kaji_fragment_marshall(kaji_t* ctx, kaji_fragment_t* f) {
	if (NULL == f) return NULL;

	f->data = kaji_marshall(ctx, f->offset, f->size);

	return f->data;
}

int
kaji_fragment_sync(kaji_t* ctx, const kaji_fragment_t* f, uint8_t block) {
	uint64_t page = kaji_fragment_page(f);
	uint64_t page_start_offset = 0 < page
		? page * KAJI_PAGE_SIZE - 1
		: 0
		;
	void* page_address = ctx->memory + page_start_offset;
	uint64_t size_padding = f->offset - page_start_offset;
	size_t size = f->size + size_padding;
	int flags = block ? MS_SYNC : MS_ASYNC;

	return msync(page_address, size, flags);
}

void*
kaji_spell(kaji_t* ctx, uint64_t offset, const uint8_t * const data, uint64_t size) {
	if (NULL == ctx || NULL == data) return NULL;

	uint8_t* start_addr = ctx->memory + offset;
	if (ctx->size < (offset + size)) {
		return NULL;
	}

	return memcpy(start_addr, data, size);
}

int
kaji_sync(kaji_t* ctx, uint8_t block) {
	return msync(ctx->memory, ctx->size, block ? MS_SYNC : MS_ASYNC);
}

const char*
kaji_sync_error(int error_code) {
	switch(error_code) {
		case EBUSY:
			return "Memory lock is busy. (EBUSY)";
		case EINVAL:
			return "Not a multiple of PAGESIZE. (EINVAL)";
		case ENOMEM:
			return "Memory (or part of it) was not mapped.";
	}

	return "Unknown error code.";
}