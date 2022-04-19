// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file fragment.h
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#include <stdint.h>

#ifndef _H_KAJI_FRAGMENT_H_
#define _H_KAJI_FRAGMENT_H_

struct kaji;

/**
 * @brief      Represents a marshalled memory fragment.
 */
struct kaji_fragment {
	void* data;
	uint64_t offset;
	uint64_t size;
};
/**
 * Convenice typedef for kaji_fragment.
 * @see struct kaji_fragment
 */
typedef
	struct kaji_fragment
	kaji_fragment_t
	;

/**
 * @brief      Gets memory page this fragment is starting in.
 *
 * @param[in]  fragment  The fragment
 *
 * @return     Memory page index.
 */
uint64_t
kaji_fragment_page(const struct kaji_fragment* fragment);

/**
 * @brief      Find fragment information for given memory pointer.
 *
 * @param      ctx   Kaji context.
 * @param      f     Fragment to hold the resulting information.
 * @param[in]  data  Memory pointer to analyse.
 *
 * @return     0 if fragment has been found. 0 < otherwise.
 */
uint8_t
kaji_fragment_find(struct kaji* ctx, struct kaji_fragment* f, const void* data);

/**
 * @brief      Marshalls a given memory range.
 * 
 * @param      ctx       The context
 * @param      fragment  The fragment
 *
 * @return     Pointer to the memory range if successful. NULL otherwise.
 */
void*
kaji_fragment_marshall(struct kaji* ctx, struct kaji_fragment* fragment);

/**
 * @brief      Instructs kaji to make sure that given fragment is synced.
 *
 * @param      ctx       The context
 * @param[in]  fragment  The fragment
 * @param[in]  block     The block
 *
 * @return     0 on success. 0 < otherwise.
 */
int
kaji_fragment_sync(struct kaji* ctx, const struct kaji_fragment* fragment, uint8_t block);

#endif