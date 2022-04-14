// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file spirits.h
	\brief Meta data structure to account for memory usage.

	^^
*/

#ifndef _H_KAJI_SPIRITS_H_
#define _H_KAJI_SPIRITS_H_

#include <stdint.h>

struct spirits {
	uint64_t offset;
	uint64_t size;
	uint8_t available;
	struct spirits* next;
	struct spirits* prev;
};
typedef
	struct spirits
	spirits_t
	;

void
spirits_initialize_memory_functions();

uint64_t
spirits_size(spirits_t* spirits, uint8_t only_this_node);

void
spirits_print_all(spirits_t* spirits);

void
spirits_print(spirits_t* spirits, uint8_t newline);

spirits_t*
spirits_rift(spirits_t* spirits, uint64_t requested_size);

uint8_t
spirits_unify_next(spirits_t* spirits);

void
spirits_banish(spirits_t* spirits);

spirits_t*
spirits_find_smallest_fit(spirits_t* spirits, uint64_t size);

uint8_t
spirits_condense(spirits_t* spirits);

uint8_t
spirits_allocate(spirits_t* spirits, uint64_t* address, uint64_t size);

void
spirits_free(spirits_t* spirits, uint64_t address);

#endif