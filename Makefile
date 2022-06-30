CC := gcc
LIBS := -lasound -lm

all: build/sound_trigger

build/sound_trigger: sound_trigger.c
	mkdir -p build/
	${CC} sound_trigger.c -o $@ ${LIBS}

clean:
	rm -rf build/
