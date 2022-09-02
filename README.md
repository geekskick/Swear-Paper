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

## results
![alt text](results/assfukka.jpg "assfukka")
![alt text](results/cunts.jpg "cunts")
![alt text](results/fekker.jpg "fekker")
![alt text](results/fagging.jpg "fagging")
![alt text](results/motherfuckers.jpg "motherfuckers")
![alt text](results/buttplug.jpg "buttplug")
![alt text](results/testicles.jpg "grope")
![alt text](results/blue_waffle.jpg "blue waffle")
![alt text](results/bitches.jpg "bitches")
![alt text](results/jelly_donut.jpg "jelly donut")

