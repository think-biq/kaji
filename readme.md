# kaji

Use memory mapped files for IPC or virtual program memory (heap).

## Overview

Kaji seeks to provide a straigh forward interface for the purpose for inter-process
communication through shared memory, specifically memory mapped files.

## Documentation

You'll find the latest documentation at [kaji.think-biq.com](https://kaji.think-biq.com).

## Example

See [examples/counter.c](examples/counter.c) for a full example.
For a simple IPC example, checkout [think-biq/kaji-example-sdl](https://github.com/think-biq/kaji-example-sdl).

### Marshalling memory

This example showcases how to directly map ranges of memory.

```c
// Creates a new kaji memory management instance.
kaji_t* kaji = kaji_materialize();
// Binds kaji to a file serving as actual memory backing for its operations.
if (0 != kaji_bind(kaji, "/tmp/kaji.ipc", 1024)) {
	fprintf(stderr, "Error binding :/ (errno: %i, %s)\n"
		, errno, strerror(errno));
	fprintf(stderr, "Could not bind file /tmp/kaji.ipc! Make sure it exists and"
					"has a size of at least 1024 bytes.\n");
	return 1;
}

// Marshalls memory beginning at offset 0 with size of an unsigned 8 byte integer.
// There memory is not account for. There is only a sanity check happening regarding
// the offset and size of the memory area. Useful if you know exactly what the layout
// will be like.
uint64_t* number = kaji_marshall(kaji, 0, sizeof(uint64_t));

// This will store the value directly in the virtual memory.
*number = 521374332;

// Discard all memory and unbind the mapped file.
if (0 != kaji_release(kaji)) {
	fprintf(stderr, "Error releasing :/ (errno: %i, %s)\n"
		, errno, strerror(errno));
}

// Discards the kaji instance.
kaji_dematerialize(kaji);
```

### Replacing heap memory functions

Using *gedanken.h* you can change the allocation behavior of malloc, calloc and realloc (as well as free of course).

```c
// Call this before any usage of memory functions. This will setup the redefinitions
// as well as prepare and bind the virtual memory file.
if (0 != gedanken_initialize(1024, "gedanken.ipc")) {
	printf("Could not setup gedanken!\n");
	return 1;
}

// Uses kaji + spirits memory allocator in the background.
char* message = malloc(sizeof(char) * 420);
free(message);

// Switch back to system memory management.
gedanken_activate(0);

if (gedanken_is_activated()) {
	// Only do this when kaji is active.
}

// Unbind and discard memory and deactivate redefinitions.
gedanken_shutdown();
```