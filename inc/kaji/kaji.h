// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file kaji.h
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#include <stdint.h>

struct kaji;
typedef
	struct kaji
	kaji_t
	;

struct kaji_fragment {
	void* data;
	uint64_t offset;
	uint64_t size;
};
typedef
	struct kaji_fragment
	kaji_fragment_t
	;

void
kaji_print_spirits(kaji_t* ctx);

kaji_t*
kaji_materialize();

void
kaji_dematerialize(kaji_t* ctx);

const char*
kaji_path(kaji_t* ctx);

uint64_t
kaji_fragment_page(const kaji_fragment_t* fragment);

uint8_t
kaji_fragment_find(kaji_t* ctx, kaji_fragment_t* f, const void* data);

uint8_t
kaji_bind(kaji_t* ctx, const char* path, uint64_t size);

uint8_t
kaji_release(kaji_t* ctx);

uint8_t
kaji_zero(const char* path, uint64_t size);

void
kaji_blank(kaji_t* ctx);

void*
kaji_allocate(kaji_t* ctx, uint64_t size);

void
kaji_free(kaji_t* ctx, void* data);

void*
kaji_marshall(kaji_t* ctx, uint64_t start, uint64_t size);

void*
kaji_fragment_marshall(kaji_t* ctx, kaji_fragment_t* fragment);

void*
kaji_spell(kaji_t* ctx, uint64_t offset, const uint8_t * const data, uint64_t size);

int
kaji_sync(kaji_t* ctx, uint8_t block);

int
kaji_sync_fragment(kaji_t* ctx, const kaji_fragment_t* fragment, uint8_t block);

const char*
kaji_sync_error(int error_code);