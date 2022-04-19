

#ifndef _H_KAJI_LOG_H_
#define _H_KAJI_LOG_H_

#include <stdio.h>

uint8_t kaji_log_active();

void kaji_log_activate();
void kaji_log_deactivate();

#define KAJI_LOG(...) \
	{ if (kaji_log_active()) { fprintf(stderr, __VA_ARGS__); } }

#endif