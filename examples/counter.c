
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

#include <tempora/all.h>
#include <spirits/all.h>
#include <kaji/kaji.h>

static uint8_t keep_running = 1;
void
handle_signal(int signal_type) {
	printf("Got signal: %i\n", signal_type);
	keep_running = 0;
}

int
msleep(unsigned int tms) {
	return usleep(tms * 1000);
}

//
struct mapped_data {
	int32_t position[3];
	uint8_t flag;
	uint16_t timing;
	char name[128];
};
typedef
	struct mapped_data
	mapped_data_t
	;
static mapped_data_t default_mapped_data = {0};

void
printf_mapped_data(const mapped_data_t* memory);

uint8_t
treat_memory(int mode, kaji_t* kaji);

void
output_permissions(mode_t m);

int main(int argc, char** argv) {
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	char tmppath[L_tmpnam];
	if (TEMPORA_ERROR == tempora_read(tmppath, L_tmpnam)) {
		printf(":/\n");
		return -1;
	}

	size_t l = strlen(tmppath);
	if (tmppath[l-1] == '/') {
		strcat(tmppath, "hope.ipc");
	}
	else {
		strcat(tmppath, "/hope.ipc");
	}

	printf("Setup mapping file at: %s\n", tmppath);

	struct stat fs;
	stat(tmppath, &fs);
	output_permissions(fs.st_mode);
	printf("filesize: %lld\n", fs.st_size);

	uint64_t bind_size = 1*1024*1024; // 16MB
	kaji_t* kaji = kaji_materialize();
	fprintf(stderr, "Binding ...\n");
	while (0 != kaji_bind(kaji, tmppath, bind_size)) {
		fprintf(stderr, "Error binding :/ (errno: %i, %s)\n"
			, errno, strerror(errno));
		if (ENOMEM == errno) {
			fprintf(stderr, "Resizing / filling %llu bytes ...\n", bind_size);
			kaji_zero(tmppath, bind_size);
			fprintf(stderr, "Retrying binding ...\n");
		}
		else {
			return 13;
		}
	}

	if (1 < argc) {
		int mode = atoi(argv[1]);
		treat_memory(mode, kaji);
	}
	else {
		printf("No mode provided :/\n");
	}

	kaji_print_spirits(kaji);

	mapped_data_t* m = kaji_allocate(kaji, 3*sizeof(mapped_data_t));
	for (uint8_t i = 0; i < 3; ++i) {
		strcpy(m[i].name, "Hanswurst ißt gerne Senf!");
		m[i].flag = i;
		m[i].position[0] = 0x11*i;
		m[i].position[1] = 0x22*i;
		m[i].position[2] = 0x33*i;
		m[i].timing = 0;
	}

	printf("Before allocation:\n");
	kaji_print_spirits(kaji);

	char* keks = kaji_allocate(kaji, sizeof(char) * 900);
	if (NULL == keks) {
		printf("ARRGH\n");
		return 42;
	}
	strcpy(keks, "God's love is the answer\n\nHe holds the key to every door\n\nSurely He has the answer\n\nyour heart is searching for\n\n \n\nThere is a resting place\n\nfor our sorrows and tears\n\nA place of peace and comfort\n\nTo toss on Him our cares\n\n \n\nThink, how the Savior suffered\n\nthat we can know today\n\nHow His heart broke in sadness\n\nto take all our burdens away\n\n \n\nHis love never says I've no time\n\nyou, His child, His precious own.\n\nTho you walk, through low valley's,\n\nHe's there, you'll never walk alone\n\n \n\nIf I could give you something\n\nit would be my darling Savior.\n\nI tell you of His caring heart,\n\nTrue to love His own forever.\n\n \n\nHe's waiting with arm's open,\n\njust for you to draw near.\n\nHe's there to lift your spirit,\n\nand put in a bottle all your tears.");

	printf("After allocation:\n");
	kaji_print_spirits(kaji);

	printf("Freeing array ...\n");
	kaji_free(kaji, m);

	printf("After free:\n");
	kaji_print_spirits(kaji);

	printf("Releasing ...\n");
	if (0 != kaji_release(kaji)) {
		fprintf(stderr, "Error releasing :/ (errno: %i, %s)\n"
			, errno, strerror(errno));		
	}

	printf("Dematerializing ...\n");
	kaji_dematerialize(kaji);

	return 0;
}

uint8_t
treat_memory(int mode, kaji_t* kaji) {
	kaji_fragment_t one = {
		.offset = 0,
		.size = sizeof(mapped_data_t)
	};
	kaji_fragment_marshall(kaji, &one);
	mapped_data_t* memory = (mapped_data_t*)one.data;
	if (NULL == memory) {
		printf("arrgh :/\n");
		return 1;
	}

	kaji_fragment_t two = {
		.offset = 3*sizeof(mapped_data_t),
		.size = sizeof(mapped_data_t)
	};
	if (NULL == kaji_fragment_marshall(kaji, &two)) {
		fprintf(stderr, "Could not marhal second fragment :/\n");
		return -1;
	}

	mapped_data_t* other_memory = (mapped_data_t*)two.data;
	if (NULL == other_memory) {
		printf("arrgh other :/\n");
		return 1;
	}

	if (0 == mode) {
		printf("Reading stuff to shared memory ...\n");
		printf_mapped_data(memory);
		printf("Reading another bunch of shared memory ...\n");
		printf_mapped_data(other_memory);

		while (keep_running) {
			printf("\rTiming:\t\t%u, Other timing:\t%u"
				, memory->timing, other_memory->timing);
			fflush(stdout);
			sleep(1.0);
		}
	}
	else if (1 == mode) {
		printf("Writing stuff to shared memory ...\n");
		uint64_t k = ~0;
		uint32_t* kp = kaji_spell(kaji, sizeof(mapped_data_t)*2, (uint8_t*)&k, sizeof k);
		*kp = 0xdeadf155;
		printf("kp: %08x\n", *kp);
		printf("kp: %08x\n", *(++kp));

		strcpy(other_memory->name, "кохання це відповідь! љубав је одговор! Láska je odpovědí! A szerelem a válasz!");
		other_memory->flag = 3;
		other_memory->position[0] = 0x33;
		other_memory->position[1] = 0x23;
		other_memory->position[2] = 0x77;

		strcpy(memory->name, "Hanswurst ißt gerne Senf!");
		memory->flag = 7;
		memory->position[0] = 1;
		memory->position[1] = 13;
		memory->position[2] = 23;
		memory->timing = 1;

		while (keep_running) {
			msleep(333);
			++(memory->timing);
			// Since we have access to the fragment, we instruct kaji to only
			// sync the pages holding the fragment's data.
			if (0 != kaji_sync_fragment(kaji, &one, 1)) {
				printf("Error syncing ONE: errno(%i): %s\n"
					, errno, strerror(errno));
			}

			other_memory->timing = memory->timing + 10;
			if (0 != kaji_sync_fragment(kaji, &two, 1)) {
				printf("Error syncing TWO: errno(%i): %s\n"
					, errno, strerror(errno));
			}

			//Alternativly sync the whole file.
			/*if (0 != kaji_sync(kaji, 0)) {
				printf("Error syncing file: errno(%i): %s\n"
					, errno, strerror(errno));
			}*/

			printf("\rTiming:\t%u, Other timing:\t%u"
				, memory->timing, other_memory->timing);
			fflush(stdout);
		}
	}

	return 0;
}

void
printf_mapped_data(const mapped_data_t* memory) {
	printf("Got data named: %s, flag: %u\n", memory->name, memory->flag);
	printf("Position: {"); char k = 'x';
	for (uint8_t i = 0; i < 3; ++i) {
		printf("%c:%i%s"
			, k+i
			, memory->position[i]
			, (i == 2 ? "" : ", ")
		);
	}
	printf("}\n");
	printf("Timing: %u\n", memory->timing);
}

void
output_permissions(mode_t m) {
	putchar( m & S_IRUSR ? 'r' : '-' );
	putchar( m & S_IWUSR ? 'w' : '-' );
	putchar( m & S_IXUSR ? 'x' : '-' );
	putchar( m & S_IRGRP ? 'r' : '-' );
	putchar( m & S_IWGRP ? 'w' : '-' );
	putchar( m & S_IXGRP ? 'x' : '-' );
	putchar( m & S_IROTH ? 'r' : '-' );
	putchar( m & S_IWOTH ? 'w' : '-' );
	putchar( m & S_IXOTH ? 'x' : '-' );
	putchar('\n');
}