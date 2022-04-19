// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file memory.h
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#include <stdint.h>

#ifndef _H_KAJI_MEMORY_H_
#define _H_KAJI_MEMORY_H_

struct kaji;

void*
kaji_allocate(struct kaji* ctx, uint64_t size);

void
kaji_free(struct kaji* ctx, void* data);

void*
kaji_marshall(struct kaji* ctx, uint64_t start, uint64_t size);

void*
kaji_spell(struct kaji* ctx, uint64_t offset, const uint8_t * const data, uint64_t size);

int
kaji_sync(struct kaji* ctx, uint8_t block);

const char*
kaji_sync_error(int error_code);

#endif