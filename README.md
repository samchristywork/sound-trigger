![Banner](https://s-christy.com/status-banner-service/sound-trigger/banner-slim.svg)

## Overview

`sound_trigger` is a program that calculates the root mean square of the audio
coming into your microphone, and can be used as a "noise detector".

```
./build/sound_trigger -h
Usage: ./build/sound_trigger [-t threshold (0-255)] [-o output file]
 -h     Display this usage statement.
 -S     Exit when threshold hit.
```

## Dependencies

In principle, you can use any C compiler and you don't have to use `make` as
the build system, but `libasound2-dev` is required because we make use of the
header file `alsa/asoundlib`.

```
gcc
libasound2-dev
make
```

## Build

```
make
```

## Run

```
./build/sound_trigger
```

## Example

Here is an example that shows normal output without setting a threshold. The
first number is the RMS amplitude, and the rest is the timestamp (indentation
added by me).

```
$ ./build/sound_trigger
  3.198968: 2022-05-28 18:58:53
 71.058899: 2022-05-28 18:58:53
131.217392: 2022-05-28 18:58:53
 89.250198: 2022-05-28 18:58:53
108.351715: 2022-05-28 18:58:54
108.496033: 2022-05-28 18:58:54
  0.000000: 2022-05-28 18:58:54
 12.599874: 2022-05-28 18:58:55
 54.046967: 2022-05-28 18:58:55
104.454010: 2022-05-28 18:58:55
109.348839: 2022-05-28 18:58:55
 99.047989: 2022-05-28 18:58:55
101.734947: 2022-05-28 18:58:55
```

This example demonstrates the "trigger" functionality. This could be useful if
you want to perform an action when the ambient noise becomes too loud for some
reason.

```
$ ./build/sound_trigger -t 100 -S && echo "Threshold Reached!"
134.172943: 2022-05-28 19:01:41
Threshold Reached!
```

## License

This work is licensed under the GNU General Public License version 3 (GPLv3).

[<img src="https://s-christy.com/status-banner-service/GPLv3_Logo.svg" width="150" />](https://www.gnu.org/licenses/gpl-3.0.en.html)
