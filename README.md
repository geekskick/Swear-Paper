# Swear-Paper
Wallpaper Generation

A command line application to generate a new desktop wallpaper, using the newest post to /r/earthporn and google's naughty word list.

## Dependancies

Basically just docker, but if you want to know more then: 
See [conafile](conanfile.txt) and [dockerfile](./docker/Dockerfile).

## Building

```bash
docker build -t swear_paper ./docker 
./rundocker.sh cmake -Bbuild . 
./rundocker.sh cmake --build build --target all
```

## Usage

```bash
./swear_paper <option>
Allowed Options:
  -h [ --help ]                         Display help message
  -s [ --source ] arg (=https://raw.githubusercontent.com/LDNOOBW/List-of-Dirty-Naughty-Obscene-and-Otherwise-Bad-Words/master/en)
                                        Specify the location of the swear word 
                                        list
  -o [ --output ] arg                   Output filename
  -q [ --quiet ]                        Don't show info messages
  --skip arg (=0)                       Skip to the nth image in the list of 
                                        available ones
  --thickness arg (=1)                  Thickness of the line used to print the
                                        word
```

## Results
![alt text](Results/assfukka.jpg "assfukka")
![alt text](Results/cunts.jpg "cunts")
![alt text](Results/fekker.jpg "fekker")
![alt text](Results/fagging.jpg "fagging")
![alt text](Results/motherfuckers.jpg "motherfuckers")
![alt text](Results/buttplug.jpg "buttplug")
![alt text](Results/testicles.jpg "grope")
![alt text](Results/blue_waffle.jpg "blue waffle")
![alt text](Results/bitches.jpg "bitches")
![alt text](Results/jelly_donut.jpg "jelly donut")

