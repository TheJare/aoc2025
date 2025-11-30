ifeq ($(OS),Windows_NT)
	EXE_EXTENSION := .exe
else
	EXE_EXTENSION :=
endif
CXX := clang++
CXXFLAGS := -std=c++23 -O2 -g
SRC := $(wildcard *.cpp)
OUT_DIR = out/
OUT := $(addprefix $(OUT_DIR), $(SRC:.cpp=${EXE_EXTENSION}))
INPUT := $(SRC:.cpp=.txt)
PCH := $(OUT_DIR)base.h.gch
URL_BASE := https://adventofcode.com/2025/day

.PHONY: all clean

all: $(OUT_DIR) $(OUT) $(INPUT)

%.gif: %/frame0000.ppm
	ffmpeg -y -i $*/frame%04d.ppm -filter_complex "[0:v] fps=30[x]; [x]split[a][b]; [a]palettegen=max_colors=256:stats_mode=full[p]; [b][p]paletteuse=dither=bayer:bayer_scale=5" $@

%.txt:
	curl --cookie=session=$(AOC_SESSION_COOKIE) $(URL_BASE)/$*/input -f -o $@

$(OUT_DIR)%${EXE_EXTENSION}: %.cpp $(PCH)
	$(CXX) $(CXXFLAGS) -include-pch $(PCH) $< -o $@

$(PCH): base.h
	$(CXX) $(CXXFLAGS) -x c++-header $< -o $@

MKDIRP = mkdir -p
RMR = rm -rf
NULLDEV = /dev/null

$(OUT_DIR):
	-$(MKDIRP) $@ >$(NULLDEV) 2>&1

clean:
	-$(RMR) $(OUT_DIR) >$(NULLDEV) 2>&1
	-$(RMR) output*/ >$(NULLDEV) 2>&1
