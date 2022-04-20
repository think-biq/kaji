
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <dlfcn.h> // dlsym

#include <tempora/all.h>
#include <spirits/all.h>
#include <kaji/all.h>
#include <kaji/gedanken.h>

#include "peom.h"

struct item {
	char name[32];
	uint64_t id;
	uint8_t flags;
};
typedef struct item item_t;

int main () {
	const uint64_t memory_size = 1024;
	if (0 != gedanken_initialize(memory_size, "use_gedanken.ipc")) { // 1 KB
		printf("Could not setup gedanken!\n");
	}

	kaji_log_activate();

	// Use gedanken memory management.
	if (gedanken_is_activated())
	{
		printf("Trying with gedanken ...\n");

		struct kaji* kaji = gedanken_kaji();
		if (NULL == kaji) {
			printf("Could not fetch kaji!\n");
			return 23;
		}
		const char* path = kaji_path(kaji);
		printf("Using %s as virtually mapped memory ...\n", path);
		kaji_print_spirits(kaji);

		// Uses kaji + spirits memory allocator in the background.
		char* keks = malloc(sizeof(char) * 900);
		if (NULL != keks) {
			kaji_print_spirits(kaji);
			kaji_fragment_t f;
			kaji_fragment_find(kaji, &f, keks);
			printf("Found fragment at %llu with size %llu.\n", f.offset, f.size);
			printf("Copying peom ...\n");
			strcpy(keks, kaji_examples_peom);
			printf("Love is the answer:\n%s\n", keks);
			free(keks);
		}
		else {
			printf("HEULL!\n");
		}

		kaji_print_spirits(kaji);
		item_t* items = calloc(10, sizeof(item_t));
		if (NULL != items) {
			memset(items, 0xaa, 10*sizeof(item_t));
			for (uint8_t i = 0; i < 10; ++i) {
				items[i].name[0] = '\0';
			}
			strcpy(items[1].name, "Hans Wurst");
			strcpy(items[4].name, "Arno Nühm");
			strcpy(items[9].name, "Karl Heinz");
			for (uint8_t i = 0; i < 10; ++i) {
				if ('\0' != items[i].name[0]) {
					printf("Got a name at %u: %s\n", i, items[i].name);
				}
			}
		}
		else {
			printf("Could not calloc!\n");
		}

		kaji_print_spirits(kaji);
		items = realloc(items, 5 * sizeof(item_t));
		if (NULL != items) {
			for (uint8_t i = 0; i < 5; ++i) {
				if ('\0' != items[i].name[0]) {
					printf("Got a name at %u: %s\n", i, items[i].name);
				}
			}
		}
		else {
			printf("Could not realloc!\n");
		}

		kaji_print_spirits(kaji);
	}
	else {
		printf("Skipping gedanken memory example ...\n");
	}

	// Use system memory management.
	gedanken_activate(0);
	printf("Without gedanken ...\n");
	{
		// Uses system defaut memory allocator.
		char* keks = malloc(sizeof(char) * 900);
		if (NULL != keks) {
			printf("Copying peom ...\n");
			strcpy(keks, kaji_examples_peom);
			printf("Peom with size: %lu\n", strlen(keks));
			free(keks);
		}
		else {
			printf("HEULL!\n");
		}
	}

	gedanken_shutdown();

	return 0;
}

