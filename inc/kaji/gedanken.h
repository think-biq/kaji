// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file gedanken.h
	\brief Utility to use memory mapped files as virtual heap.

	^^
*/

#ifndef _H_GEDANKEN_H_
#define _H_GEDANKEN_H_

#ifdef USE_GEDANKEN

#include <stdint.h>
#include <stdlib.h>

struct kaji;

int gedanken_initialize(uint64_t head_size, const char* filename);

struct kaji* gedanken_kaji();

uint8_t gedanken_is_activated();

void gedanken_activate(uint8_t active);

void* malloc(size_t size);

void free(void* memory);

void gedanken_shutdown();

#else

#warning "Disabling gedanken. Using stubs for API calls!"

#define gedanken_initialize(...) 1
#define gedanken_kaji() NULL
#define gedanken_is_activated() 0
#define gedanken_activate(...) {}
#define gedanken_shutdown(...) {}

#endif

#endif