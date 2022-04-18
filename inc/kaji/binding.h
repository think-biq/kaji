// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file binding.h
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#include <stdint.h>

#ifndef _H_KAJI_COMMON_H_
#define _H_KAJI_COMMON_H_

struct kaji;

uint8_t
kaji_bind(struct kaji* ctx, const char* path, uint64_t size);

uint8_t
kaji_release(struct kaji* ctx);

#endif