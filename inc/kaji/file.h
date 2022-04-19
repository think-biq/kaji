// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file file.h
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#ifndef _H_KAJI_FILE_H_
#define _H_KAJI_FILE_H_

#include <stdint.h>

/**
 * @brief      Creates an IPC file at path with at least size.
 *
 * @param[in]  path  The path
 * @param[in]  size  The size
 *
 * @return     0 on success. 0 < otherwise.
 */
uint8_t
kaji_file_create(const char* path, uint64_t size);

/**
 * @brief      Expands an IPC file at path to at least size.
 *
 * @param[in]  path  The path
 * @param[in]  size  The size
 *
 * @return     0 on success. 0 < otherwise.
 */
uint8_t
kaji_file_expand(const char* path, uint64_t size);

/**
 * @brief      Overwrites IPC file with 0s.
 *
 * @param[in]  path  The path
 *
 * @return     0 on success. 0 < otherwise.
 */
uint8_t
kaji_file_zero(const char* path);


#endif