# Swear-Paper
Wallpaper Generation

A command line application to generate a new desktop wallpaper, using the newest post to /r/earthporn and google's naughty word list.

## Dependancies

1. libcurl
2. opencv
3. nhollmans json header

## Building

When in the directory containing the xcode project file (if thats what you're using):

```bash
xcodebuild build
```

OR

```bash
g++ main.cpp image.cpp reddit_interface.cpp earthporn.cpp downloader.cpp -o test_swear_paper -std=c++14 -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -L/usr/lib -lcurl
```

OR

```
mkdir build
make
```

## Usage

./swear_paper <outputfilename>

## Results
![alt text](Results/assfukka.jpg "assfukka")
![alt text](Results/cunts.jpg "cunts")
![alt text](Results/fekker.jpg "fekker")
![alt text](Results/fagging.jpg "fagging")
![alt text](Results/motherfuckers.jpg "motherfuckers")
![alt text](Results/buttplug.jpg "buttplug")
![alt text](Results/testicles.jpg "grope")

