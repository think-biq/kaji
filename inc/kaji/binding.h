// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file binding.h
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#ifndef _H_KAJI_COMMON_H_
#define _H_KAJI_COMMON_H_

#include <stdint.h>

struct kaji;

/**
 * @brief      Binds given file to kaji's memory managment.
 *
 * @param      ctx   Pointer to the kaji context object.
 * @param[in]  path  Path to the file to be bound.
 * @param[in]  size  Size of the memory to be used after binding.
 *
 * @return     0 on success. Otherwise sets errno and returns a value > 0.
 */
uint8_t
kaji_bind(struct kaji* ctx, const char* path, uint64_t size);

/**
 * @brief      Releases the currently bound file and frees the memory.
 *
 * @param      ctx   The context
 *
 * @return     { description_of_the_return_value }
 */
uint8_t
kaji_release(struct kaji* ctx);

/**
 * @brief      Retrieves the path of the currently mapped file.
 *
 * @param      ctx   Pointer to the kaji context object.
 *
 * @return     NULL if no file is mapped. Path otherwise.
 */
const char*
kaji_path(kaji_t* ctx);

#endif