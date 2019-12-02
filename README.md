# Cpp-image-alignment-2dcamera
C++ OpenCV project to align an object with a 2D camera based on image data

### Building the Project
To build this project, first clone the repo.
```bash
mkdir build && cd build
cmake ..
make
./AlignImage
```

### Purpose
The purpose of this tool will be to allow the user, through repeated use of the tool, to take an image of an object and until it aligns horizontally and vertically with the edges of a camera. 

This is a known issue on production lines (particularly displays) where the accuracy of camera measurements depends on camera alignment.

The aim of the project is to read in images of an object and tell the user whether the camera is square with the object.
Conceptually, this is similar to a spirit-level, it will tell the user if an object is tilted to left, right, or up or down.
An end-view or side-view of the object is needed, where the program will get the 4 corners of the object and output the different between x-plane and the y-plane of each corner. With a rectangular object, this means that the left (top and bottom corners) are on the same plane, similar to the the right (top and bottom) and the top corner (left and right), etc.

### Requirements
This project is the Capstone submission for the Udacity nanodegree program, which requires several features:
* Demonstates an understanding of C++ functions and control structures
* File IO
* Well design OOP incorporating abstraction, encapsulation and inheritance.
* Use of proper memory management - references, RAII, move semantics and smart pointers.
* Use of concurrent programming - mutlithreading is utilized safely through promises and futures, mutexes and locks.

### Features & Usage
The intent of the program is to read in an image, process it, overlay lines over the edges of an object and output the resulting image. It should also output a CSV file, indicating the co-ordiniates of the top-left, top-right, bottom-left and bottom-right corners of the image, and the degree to which the respective planes differ as a percentage value.
The input then will be image files and the output will be image files and CSVs.

### Implementation
This project will fulfill it's requirements the following way:
#### OOP 
There will be several classes used, ImageData (for reading, cropping and writing the images), Logger (generic logger class to assist in debug), MessageQueue (to allow for concurrent programming) and ImageDataPtr (for memory management).
#### File IO
Input will be taken and parsed from a config file and output will be sent to a log file.
#### Memory Management
Each time an image is read in, memory from the heap will be allocated. When the image data is saved to a file, it will be destoryed.
#### MultiThreading
The processing of the images will be split into several section,
* Reading in the image.
* Finding the borders of the image.
* Overlaying borders on the image.
* Saving the resulting image to the output folder.

Only 1 of each of these sections will be run at a time. Each seperate image in the folder will be read in a seperate thread,
so by locking each thread until the previous thread has complete the next section, it will ensure a stable cycle of image processing
and allow for ease-of-debug - and understanding.

#### Class Structure
See files in src/ for more info.

#### User Configuration
A .json file is used to read in user settings. This includes the file path for the log file and the path to the images.