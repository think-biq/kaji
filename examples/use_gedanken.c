
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

int main () {
	if (0 != gedanken_initialize(1024, "wurstwasser.ipc")) { // 1 KB
		printf("Could not setup gedanken!\n");
	}

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

