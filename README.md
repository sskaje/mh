# MH
A Memory Editor for iOS/OSX with JavaScript support

## Requirement

* mac OS / Mac OSX
* Xcode
* jailbroken iDevice if you want to try iOS versions
* [linenoise](https://github.com/antirez/linenoise.git) for interactive cli tool
* [duktape](https://github.com/svaarala/duktape) for JavaScript support


## Build & Run

### Prepare

```bash
$ git clone https://github.com/sskaje/mh
$ cd mh
# prepare linenoise
$ git submodule update --init --recursive
# prepare iPhoneOS SDKs
$ bash tools/download_sdks.sh
# Prepare duktape
$ bash tools/duktape_prepare.sh
```

### Build
```bash
# Build for OSX
$ bash build/build-osx.sh
# Build for ARMv7 iDevices
$ bash build/build-ios-armv7.sh
# Build for ARMv7s iDevices
$ bash build/build-ios-armv7s.sh
# Build for ARM64 iDevices, iOS 10.2
$ bash build/build-ios-armv64.sh
# Build for ARM64 iDevices, iOS 11.x by Electra
$ bash build/build-ios-armv64-electra.sh

```

### Run

```bash
$ sudo ./mh_cli
```

For iOS 11.x by Electra, binary must locates /bin/mh_cli

Read more: [Run HelloWorld on Jailbroken iOS 11](https://sskaje.me/2018/03/run-helloworld-on-jailbroken-ios-11/)


## Documentation

### CLI Commands

```
=== Memory Hacker ===
Author: sskaje

Command                                 Description
 process-list                            Print process list
 open PID                                Open a task
 close                                   Close current task
 search-hex HEX                          Search HEX bytes, like search-hex 1a2b3c4d
 update-search-hex HEX                   Search HEX from previous result
 search-string STRING                    Search string, like search-string hello
 update-search-string STRING             Search string from previous result
 memory-read ADDRESS SIZE                Read memory data at ADDRESS, both ADDRESS and SIZE are in HEX
 memory-write ADDRESS STR                Write data to ADDRESS, ADDRESS in HEX, STR in BYTES
 memory-write-hex ADDRESS HEXSTR         Write hex data to ADDRESS, both ADDRESS and HEXSTR are in HEX
 result                                  Print result list, last round by default
 dyld                                    Print dyld info
 vm-region                               Print mach_vm_region()
 bytes2hex STRING                        Bytes to hex string
 hex2bytes HEXSTR                        Hex string to bytes
 float2hex FLOAT                         Float number to hex
 double2hex DOUBLE                       Double number to hex
 int2hex INTEGER                         Integer to hex
 clear                                   Clear screen
 help                                    Print this message
 
 ```

Example:
```
$ mh_cli
MH[nil] > process-list
...
Process count=415
MH[nil] > open 379
Current PID=379
MH[379] > search-string 0ABCDEFGHIJKLMN0
Found 3 result(s).
MH[379] > result
...
update search @address:000000008516f810
[2] 0x0000000085100000-0x0000000085200000 size=0x7ffe00000020 offset=0000000000000000, rw-/rwx, MALLOC_TINY
  000000008516f810  30 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 30  0ABCDEFGHIJKLMN0
  000000008516f820  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
MH[379] > memory-write 000000008516f810 "Hi, MH"
  0000000000000000  48 69 2c 20 4d 48                                Hi, MH
Write memory: addr=000000008516f810, size=0x6
MH[379] > result
...
[2] 0x0000000085100000-0x0000000085200000 size=0x7ffe00000020 offset=0000000000000000, rw-/rwx, MALLOC_TINY
  000000008516f810  48 69 2c 20 4d 48 46 47 48 49 4a 4b 4c 4d 4e 30  Hi, MHFGHIJKLMN0
  000000008516f820  00 00 00 00 00 00 00 00 00 00 00 00 00 00 04 00  ................
MH[379] > memory-write-hex 000000008516f818 AFBDEECC3231
  0000000000000000  af bd ee cc 32 31                                ....21
Write memory: addr=000000008516f818, size=0x6
MH[379] > result
...
[2] 0x0000000085100000-0x0000000085200000 size=0x7ffe00000020 offset=0000000000000000, rw-/rwx, MALLOC_TINY
  000000008516f810  48 69 2c 20 4d 48 46 47 af bd ee cc 32 31 4e 30  Hi, MHFG....21N0
  000000008516f820  00 00 00 00 00 00 00 00 00 00 00 00 00 00 04 00  ................
```


### JavaScript API

See [docs/define.js](https://github.com/sskaje/mh/blob/master/docs/define.js)

Example:
  [tests/test-script.js](https://github.com/sskaje/mh/blob/master/tests/test-script.js)


## Known issues



## License

[MH: MIT](https://github.com/sskaje/mh/blob/master/LICENSE)

[linenoise](https://github.com/antirez/linenoise/blob/master/LICENSE)

[duktape](https://github.com/svaarala/duktape/blob/master/LICENSE.txt)

[Cydia](https://github.com/ElectraJailbreak/cydia/blob/master/COPYING) if you're using mh for iOS 11 jailbroken by Electra.

