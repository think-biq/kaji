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

/**
 * @brief      Initializes gedanken memory function replacment.
 * 
 * This sets up a kaji memory managment instance bound to a file with the
 * name specified throug 'filename' and the specified size. After this, the 
 * default memory functions malloc, calloc and free are shadowed and redirected
 * to use kaji memory instead of system memory. You can control which memory is
 * used via gedanken_activate. When you are finished working with gedanken,
 * call gedanken_shutdown.
 * 
 * @see gedanken_activate
 * @see gedanken_shutdown
 *
 * @param[in]  memory_size  The size of the memory to be prepared.
 * @param[in]  filename   The filename for the memory mapped file.
 *
 * @return     0 on success. 0 < otherwise.
 */
int gedanken_initialize(uint64_t memory_size, const char* filename);

/**
 * @brief      Retrieve the backing kaji memory management instance.
 * 
 * @see gedanken_initialize
 *
 * @return     Kaji instance or NULL if gedanken has not been initialized.
 */
struct kaji* gedanken_kaji();

/**
 * @brief      Checks whether gedanken is currently active or not.
 * 
 * Can be change via gedanken_activate.
 * 
 * @see gedanken_activate
 *
 * @return     0 < if active, 0 otherwise.
 */
uint8_t gedanken_is_activated();

/**
 * @brief      Sets gedanken active of inactive.
 * 
 * You can retrieve the current state via gedanken_is_activated.
 * 
 * @see gedanken_is_activated
 *
 * @param[in]  active  Sets the state of gedanken.
 */
void gedanken_activate(uint8_t active);

/**
 * @brief      Shuts down gedanken system and releases all memory.
 */
void gedanken_shutdown();

void* malloc(size_t size);

void* calloc(size_t number_of_elements, size_t element_size);

void* realloc(void* data, size_t size);

void free(void* memory);

#else

#if defined(WIN32)
#pragma message( "Disabling gedanken. Using stubs for API calls!" )
#else
#warning "Disabling gedanken. Using stubs for API calls!"
#endif

#define gedanken_initialize(...) 1
#define gedanken_kaji() NULL
#define gedanken_is_activated() 0
#define gedanken_activate(...) {}
#define gedanken_shutdown(...) {}

#endif

#endif