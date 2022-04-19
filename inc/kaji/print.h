// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file print.h
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#ifndef _H_KAJI_PRINT_H_
#define _H_KAJI_PRINT_H_

struct kaji;

/**
 * @brief      Prints the current kaji spirit layout.
 *
 * @param      ctx   Pointer to the kaji context object.
 */
void
kaji_print_spirits(struct kaji* ctx);

#endif