# 2021-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
# See license.md for details.

all: prepare build
all-release: prepare-release build
examples: all build-examples

prepare:
	cmake -B staging -DCMAKE_BUILD_TYPE=Debug -DUSE_GEDANKEN=1

prepare-release:
	cmake -B staging -DUSE_GEDANKEN=1

build:
	cmake --build staging

build-examples:
	cmake --build staging --target kaji-example-counter
	cmake --build staging --target kaji-example-gedanken

clean:
	rm -rf staging

run:
	staging/./kaji-example-counter $(MODE)

symbols:
	nm -an staging/kaji-example-gedanken | c++filt