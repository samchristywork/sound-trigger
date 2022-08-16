CC := gcc
LIBS := -lasound -lm

all: build/sound_trigger

build/sound_trigger: src/sound_trigger.c
	mkdir -p build/
	${CC} src/sound_trigger.c -o $@ ${LIBS}

clean:
	rm -rf build/

.PHONY: clean
