// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file file.h
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#include <stdint.h>

#ifndef _H_KAJI_FILE_H_
#define _H_KAJI_FILE_H_

uint8_t
kaji_file_create(const char* path, uint64_t size);

uint8_t
kaji_file_expand(const char* path, uint64_t size);

uint8_t
kaji_file_zero(const char* path);


#endif