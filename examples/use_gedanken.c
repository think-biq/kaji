
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
#include <kaji/kaji.h>
#include <kaji/gedanken.h>

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

		char* keks = malloc(sizeof(char) * 900);
		if (NULL != keks) {
			kaji_print_spirits(kaji);
			kaji_fragment_t f;
			kaji_fragment_find(kaji, &f, keks);
			printf("Found fragment at %llu with size %llu.\n", f.offset, f.size);
			printf("Copying peom ...\n");
			strcpy(keks, "God's love is the answer\n\nHe holds the key to every door\n\nSurely He has the answer\n\nyour heart is searching for\n\n \n\nThere is a resting place\n\nfor our sorrows and tears\n\nA place of peace and comfort\n\nTo toss on Him our cares\n\n \n\nThink, how the Savior suffered\n\nthat we can know today\n\nHow His heart broke in sadness\n\nto take all our burdens away\n\n \n\nHis love never says I've no time\n\nyou, His child, His precious own.\n\nTho you walk, through low valley's,\n\nHe's there, you'll never walk alone\n\n \n\nIf I could give you something\n\nit would be my darling Savior.\n\nI tell you of His caring heart,\n\nTrue to love His own forever.\n\n \n\nHe's waiting with arm's open,\n\njust for you to draw near.\n\nHe's there to lift your spirit,\n\nand put in a bottle all your tears.");		
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
		char* keks = malloc(sizeof(char) * 900);
		if (NULL != keks) {
			printf("Copying peom ...\n");
			strcpy(keks, "God's love is the answer\n\nHe holds the key to every door\n\nSurely He has the answer\n\nyour heart is searching for\n\n \n\nThere is a resting place\n\nfor our sorrows and tears\n\nA place of peace and comfort\n\nTo toss on Him our cares\n\n \n\nThink, how the Savior suffered\n\nthat we can know today\n\nHow His heart broke in sadness\n\nto take all our burdens away\n\n \n\nHis love never says I've no time\n\nyou, His child, His precious own.\n\nTho you walk, through low valley's,\n\nHe's there, you'll never walk alone\n\n \n\nIf I could give you something\n\nit would be my darling Savior.\n\nI tell you of His caring heart,\n\nTrue to love His own forever.\n\n \n\nHe's waiting with arm's open,\n\njust for you to draw near.\n\nHe's there to lift your spirit,\n\nand put in a bottle all your tears.");
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

