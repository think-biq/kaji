// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file kaji.c
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <libloaderapi.h>
#include <io.h>
#include <kaji/win/mman.h>
#else
#include <sys/mman.h>
#include <dlfcn.h> // dlsym
#include <unistd.h>
#endif
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include <spirits/all.h>

#include <kaji/all.h>

#ifdef _WIN32
#define KAJI_PATH_SIZE 256
#else
#define KAJI_PATH_SIZE 4096
#endif



static inline size_t
_kaji_system_page_size() {
    size_t page_size;
	#if defined(_WIN32)
    SYSTEM_INFO sSysInfo;
    GetSystemInfo(&sSysInfo);
    page_size = sSysInfo.dwPageSize;
	#elif defined(PAGESIZE) // defined in limits.h
    page_size = PAGESIZE;
	#else                   // assume some POSIX OS
    page_size = sysconf(_SC_PAGESIZE);
	#endif
    return page_size;
} 

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

static uint8_t g_kaij_log_activated = 0;
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
		printf("Initializing kaji memory ...\n");
		#if defined(_WIN32)
		HMODULE hModule = GetModuleHandle(TEXT(UCRTBASEDLL_NAME));
		assert(NULL != hModule && "Could not get module :/");
		(FARPROC)system_malloc = GetProcAddress(hModule, 
			TEXT("malloc")
		);
		(FARPROC)system_free = GetProcAddress(hModule,
      		TEXT("free")
      	);
		#else
		system_malloc = dlsym(RTLD_NEXT, "malloc");
		system_free = dlsym(RTLD_NEXT, "free");
		#endif
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

uint8_t
kaji_log_active() {
	return g_kaij_log_activated;
}

void
kaji_log_activate() {
	g_kaij_log_activated = 1;
}

void
kaji_log_deactivate() {
	g_kaij_log_activated = 0;
}

void
_kaji_file_write_zeroes(int fd, uint64_t size) {
	static const uint64_t default_data = 0;
	static const uint64_t chunk_size = sizeof default_data;

	uint64_t cycles = size / chunk_size;
	uint64_t rest = cycles % chunk_size;
	for (uint64_t i = 0; i < cycles; ++i) {
		write(fd, &default_data, sizeof default_data);
	}
	write(fd, &default_data, rest);
}

uint8_t
_kaji_file_write_zeroes_f(FILE* f, uint64_t size) {
	static const uint64_t default_data = 0;
	static const uint64_t chunk_size = sizeof default_data;

	errno = 0;

	uint64_t cycles = size / chunk_size;
	uint64_t rest = cycles % chunk_size;
	// Write int 64bit chunks.
	for (uint64_t i = 0; i < cycles; ++i) {
		fwrite(&default_data, chunk_size, 1, f);
		if (0 != errno) {
			KAJI_LOG("(errno: %i, %s)\n", errno, strerror(errno));
			return 1;
		}
	}
	// And append the rest in individual bytes.
	for (uint64_t i = 0; i < rest; ++i) {
		fwrite(&default_data, 1, 1, f);
		if (0 != errno) {
			KAJI_LOG("(errno: %i, %s)\n", errno, strerror(errno));
			return 1;
		}
	}

	return 0;
}

uint8_t
kaji_file_create(const char* path, uint64_t size) {
	int fd = open(path, O_RDWR | O_CREAT, 0666);
	if (0 > fd) {
		return 3;
	}

	_kaji_file_write_zeroes(fd, size);

	close(fd);

	return 0;
}

uint8_t
kaji_file_expand(const char* path, uint64_t size) {
	// From: https://man7.org/linux/man-pages/man2/open.2.html
	// O_APPEND may lead to corrupted files on NFS filesystems if
	// more than one process appends data to a file at once.
	// This is because NFS does not support appending to a file,
	// so the client kernel has to simulate it, which can't be
	// done without a race condition.
	
	// That's why we open it as read/write.
	//int fd = open(path, O_RDWR | O_APPEND, 0666);
	//if (0 > fd) {
	//	return 1;
	//}

	// Well here we go regardless.
	FILE* f = fopen(path, "ab+");
	if (NULL == f) {
		return 1;
	}

	// Seek to the end.
	fseek(f, 0, SEEK_END);
	long current_size = ftell(f);
	if (0 > current_size) {
		return 2;
	}

	// And if necessary,
	if (size <= (uint64_t)current_size) return 0;

	// just keep writing the missing bytes.
	uint64_t expansion = size - (uint64_t)current_size;
	KAJI_LOG("Expanding file with %llu bytes from %llu to %llu ...\n"
		, expansion
		, (uint64_t)current_size
		, size
	);

	if (0 != _kaji_file_write_zeroes_f(f, expansion)) {
		return 3;
	}

	KAJI_LOG("Closing file ...\n");
	fclose(f);
	//close(fd);

	return 0;
}

uint8_t
kaji_file_zero(const char* path) {
	int fd = open(path, O_RDWR, 0666);
	if (0 > fd) {
		return 1;
	}

	//
	FILE* f = fdopen(fd, "wb+");
	if (NULL == f) {
		return 1;
	}

	//
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	if (0 > size) {
		return 2;
	}

	fseek(f, 0, SEEK_SET);
	_kaji_file_write_zeroes_f(f, (uint64_t)size);

	fclose(f);

	close(fd);

	return 0;
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

	int32_t file_descriptor = open(path, O_RDWR, 0666);
	if (0 > file_descriptor) {
		return 3;
	}

	struct stat fs;
	stat(ctx->path, &fs);
	if (fs.st_size < size) {
		KAJI_LOG("File appears to be %llu in size, yet needs to be %llu ...\n"
			, fs.st_size
			, size
		);
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

void*
kaji_allocate(kaji_t* ctx, uint64_t size) {
	// Find the first fitting memory block for given size.
	uint64_t offset = 0;
	if (0 == spirits_allocate(&(ctx->spirits), &offset, size)) {
		KAJI_LOG("Allocated %llu bytes of memory at offset: %llu\n", size, offset);
	}
	else {
		KAJI_LOG("Allocation error :/\n");
		errno = ENOMEM;
		return NULL;
	}

	// Double check that block fits into virtual memory area.
	if (ctx->size < (offset + size)) {
		KAJI_LOG("Memory range outside of mapped file bounds!\n");
		errno = ENOMEM;
		return NULL;
	}

	// Return pointer to beginning of the memory area.
	return (void*)(ctx->memory + offset);
}

void
kaji_free(kaji_t* ctx, void* data) {
	kaji_fragment_t f;
	if (0 == kaji_fragment_find(ctx, &f, data)) {
		spirits_free(&(ctx->spirits), f.offset);
	}
	else {
		KAJI_LOG("Could not find fragment for data :/\n");
	}
}

void*
kaji_marshall(kaji_t* ctx, uint64_t offset, uint64_t size) {
	if (NULL == ctx || NULL == ctx->memory) {
		KAJI_LOG("Context invalid!\n");
		errno = EINVAL;
		return NULL;
	}

	if (ctx->size < (offset + size)) {
		KAJI_LOG("Memory range outside of mapped file bounds!\n");
		errno = ENOMEM;
		return NULL;
	}

	return (void*)(ctx->memory + offset);
}

uint64_t
kaji_fragment_page(const kaji_fragment_t* f) {
	return (uint64_t)(
		f->offset / (uint64_t)_kaji_system_page_size()
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
	if (NULL == f) {
		errno = EINVAL;
		return NULL;
	}

	f->data = kaji_marshall(ctx, f->offset, f->size);

	return f->data;
}

int
kaji_fragment_sync(kaji_t* ctx, const kaji_fragment_t* f, uint8_t block) {
	uint64_t page = kaji_fragment_page(f);
	uint64_t page_start_offset = 0 < page
		? page * _kaji_system_page_size() - 1
		: 0
		;
	void* page_address = ctx->memory + page_start_offset;
	uint64_t size_padding = f->offset - page_start_offset;
	size_t size = f->size + size_padding;
	int flags = 0;
	#if defined(WIN32)
	return 1;
	#else
	flags = block ? MS_SYNC : MS_ASYNC;
	return msync(page_address, size, flags);
	#endif
}

void*
kaji_spell(kaji_t* ctx, uint64_t offset, const uint8_t * const data, uint64_t size) {
	if (NULL == ctx || NULL == data) {
		errno = EINVAL;
		return NULL;
	}

	uint8_t* start_addr = ctx->memory + offset;
	if (ctx->size < (offset + size)) {
		KAJI_LOG("Memory range outside of mapped file bounds!\n");
		errno = ENOMEM;
		return NULL;
	}

	return memcpy(start_addr, data, size);
}

int
kaji_sync(kaji_t* ctx, uint8_t block) {
	int flags = 0;
	#if defined(WIN32)
	return 1;
	#else
	flags = block ? MS_SYNC : MS_ASYNC;
	return msync(ctx->memory, ctx->size, flags);
	#endif
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