// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file core.h
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#include <stdint.h>

#ifndef _H_KAJI_CORE_H_
#define _H_KAJI_CORE_H_

/**
 * @brief      Context structure for kaji operations.
 */
struct kaji;
/**
 * @brief      Convenience typedef for context structure.
 */
typedef
	struct kaji
	kaji_t
	;

/**
 * @brief      Create new instance of a kaji system and return a context object.
 *
 * @return     Pointer to the newly created context object.
 */
kaji_t*
kaji_materialize();

/**
 * @brief      Delets given kaji instance.
 *
 * @param      ctx   Pointer to the kaji context object.
 */
void
kaji_dematerialize(kaji_t* ctx);

#endif