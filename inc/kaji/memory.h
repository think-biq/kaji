// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file memory.h
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#ifndef _H_KAJI_MEMORY_H_
#define _H_KAJI_MEMORY_H_

#include <stdint.h>

struct kaji;

/**
 * @brief      Allocates the requested memory and keeps track of it.
 * 
 * If NULL is returned, check errno for details on what went wrong.
 *
 * @param      ctx   Pointer to the kaji context object.
 * @param[in]  size  Size of the memory to be allocated.
 *
 * @return     Pointer to the allocated memory or NULL if something went wrong.
 */
void*
kaji_allocate(struct kaji* ctx, uint64_t size);

/**
 * @brief      Frees allocated memory block starting at given memory.
 *
 * @param      ctx   Pointer to the kaji context object.
 * @param      data  Pointer to block of memory.
 */
void
kaji_free(struct kaji* ctx, void* data);

/**
 * @brief      Gives access to a block of memory, without allocating it.
 *
 * Will set errno if something went wrong.
 * 
 * @param      ctx    Pointer to the kaji context object.
 * @param[in]  offset  Number of bytes from the start of kaji virtual memory.
 * @param[in]  size   Number of bytes to be accessed.
 *
 * @return     Pointer to the marshalled memory or NULL if something went wrong.
 */
void*
kaji_marshall(struct kaji* ctx, uint64_t offset, uint64_t size);

/**
 * @brief      Copies given data to kaji memory.
 * 
 * Will set errno if something went wrong.
 *
 * @param      ctx     Pointer to the kaji context object.
 * @param[in]  offset  Number of bytes from the start of kaji virtual memory.
 * @param[in]  data    Pointer to the data to be copied.
 * @param[in]  size    Number of bytes to be copied.
 *
 * @return     Pointer to the copied memory or NULL if something went wrong.
 */
void*
kaji_spell(struct kaji* ctx, uint64_t offset
    , const uint8_t * const data, uint64_t size);

/**
 * @brief      Instructs kaji to make sure all memory changes are persisted.
 * 
 * Will set errno if something went wrong.
 *
 * @param      ctx    Pointer to the kaji context object.
 * @param[in]  block  Whether to block until operation is done.
 *
 * @return     0 if success, 0 < otherwise.
 */
int
kaji_sync(struct kaji* ctx, uint8_t block);

/**
 * @brief      Creates a readable string based on given error code.
 *
 * @param[in]  error_code  The error code.
 *
 * @return     String describing the error.
 */
const char*
kaji_sync_error(int error_code);

#endif