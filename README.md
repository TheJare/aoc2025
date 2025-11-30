# Advent of Code 2025

As usual, I'm not sure if I will stick to it, but let's see.

I couldn't figure out a new programming language I wanted to learn this year, so I'm going to C++ it for old time's sake. It didn't take long to find issues with the ranges library, which is feels to me like the eternal "wouldn't it be nice if it was actually concise and even more if it actually worked." 

Makefile assumes GNU make and uses clang. `make out/1` (or `make out/1.exe` on Windows) to build that day, `make` to build all days, `make clean` to cleanup.

Have your AOC session cookie in the environment `AOC_SESSION_COOKIE` variable (just the hex contents) to download the input file for the day.

I've added a little utility to output movies, in case I get some good ideas for doing that. FFmpeg to convert to animated gif in the Makefile, use like `$ make output_02.gif`. Other formats could be generated like:

```
$ ffmpeg -i output_01/frame%04d.ppm -framerate 30 -c:v libx264 -pix_fmt yuv420p output_video.mp4
```
