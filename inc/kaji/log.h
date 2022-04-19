// 2022-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
// See license.md for details.
/*! \file log.h
    \brief Abstraction layer to use memory mapped files for IPC purposes.

    ^^
*/

#ifndef _H_KAJI_LOG_H_
#define _H_KAJI_LOG_H_

#include <stdio.h>

/**
 * @brief      Checks whether kaji verbose logging is active.
 *
 * @return     0 < if active, 0 otherwise.
 */
uint8_t kaji_log_active();

/**
 * @brief      Sets kaji verbose logging active.
 */
void kaji_log_activate();

/**
 * @brief      Sets kaji verbose logging inactive.
 */
void kaji_log_deactivate();

/**
 * @brief      Verbose logging macro.
 *
 * @param      First parameter shall be a format string, followed by paramters.
 *
 */
#define KAJI_LOG(...) \
	{ if (kaji_log_active()) { fprintf(stderr, __VA_ARGS__); } }

#endif