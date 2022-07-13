This is xmodem transfer program on Mac OS X for xmodem80.

I use this command with CP/M8266's xr and xs.

File recive

xr /dev/cu.usbmodem file.txt

File send

xs /dev/cu.usbmodem file.txt

xr only support block size(128 byte) and not use checksum.

xs and xr support standard io. This is use case on cu command.

```
f>a:xr hello.com
CP/M XR - Xmodem receive v0.1 / SmallRoomLabs 2017
~+ ./xs hello.com

File HELLO.COM received successfully
```

You must check save dirctory permition on use xr command.
