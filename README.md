## Overview

`sound_trigger` is a program that calculates the root mean square the audio
coming into your microphone, and can be used as a "noise detector".

```
./build/sound_trigger -h
Usage: ./build/sound_trigger [-t threshold (0-255)] [-o output file]
 -h     Display this usage statement.
 -S     Exit when threshold hit.
```

## Dependencies

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

```
$ ./build/sound_trigger -t 100 -S && echo "Threshold Reached!"
134.172943: 2022-05-28 19:01:41
Threshold Reached!
```
