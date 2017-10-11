# phyani_playground

## Project setup
1. Initialize all submodules
2. Generate and download glad.zip using [this link](http://glad.dav1d.de/#profile=compatibility&specification=gl&api=gl%3D4.3&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&language=c&loader=on).
3. Extract glad.zip to ./lib/glad/
4. Configure the project using CMake

## Tested environments
Working:
 - Microsoft Visual Studio 2017 Update 3 (compiler version >= 19.11)
 - GCC 7.0 on Ubuntu 17.04
 - Clang 5.0.0 with libstdc++ on Ubuntu 17.04

Not working:
 - Clang 4.x.x and older with libstdc++ on Linux