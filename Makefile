SHELL := /bin/bash

FMT_VER=12.1.0
FMT_DIR=fmt-$(FMT_VER)

all: org ios fmt std

org:
	hyperfine -m 20 "g++ -Wall -Wextra -Werror -std=c++23 -o parse_date parse_date_org.cpp"

ios:
	hyperfine -m 20 "g++ -Wall -Wextra -Werror -std=c++23 -o parse_date parse_date_ios.cpp"

fmt:
	hyperfine -m 20 "g++ -Wall -Wextra -Werror -std=c++23 -I./$(FMT_DIR)/include -L./$(FMT_DIR)/build -o parse_date parse_date_fmt.cpp -lfmt"

std:
	hyperfine -m 20 "g++ -Wall -Wextra -Werror -std=c++23 -o parse_date parse_date_std.cpp"

prepare:
	curl -sL https://github.com/fmtlib/fmt/archive/refs/tags/$(FMT_VER).tar.gz | tar xz
	pushd $(FMT_DIR) && mkdir -p build && cd build && cmake -DFMT_TEST=OFF .. && make && popd

clean:
	rm parse_date

.PHONY: all org ios fmt std prepare clean
