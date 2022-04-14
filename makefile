
all: prepare build
examples: all build-examples

prepare:
	cmake -B staging -DCMAKE_BUILD_TYPE=Debug -DUSE_GEDANKEN=1

build:
	cmake --build staging

build-examples:
	cmake --build staging --target kaji-example-counter
	cmake --build staging --target kaji-example-gedanken

clean:
	rm -rf staging

run:
	staging/./asshat $(MODE)

symbols:
	nm -an staging/asshat | c++filt