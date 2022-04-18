// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file core.h
	\brief Abstraction layer to use memory mapped files for IPC purposes.

	^^
*/

#include <stdint.h>

#ifndef _H_KAJI_CORE_H_
#define _H_KAJI_CORE_H_

struct kaji;
typedef
	struct kaji
	kaji_t
	;

const char*
kaji_path(kaji_t* ctx);

kaji_t*
kaji_materialize();

void
kaji_dematerialize(kaji_t* ctx);

#endif