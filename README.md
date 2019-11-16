# Cpp-image-alignment-2dcamera
C++ OpenCV project to align an object with a 2D camera based on image data

# Building the Project
To build this project, first clone the repo.
'''
mkdir build && cd build
cmake ..
make
./AlighImage

### Purpose
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

### Features
The intent of the program is to read in an image, process it, overlay lines over the edges of an object and output the resulting image. It should also output a CSV file, indicating the co-ordiniates of the top-left, top-right, bottom-left and bottom-right corners of the image, and the degree to which the respective planes differ as a percentage value.
The input then will be image files and the output will be image files and CSVs.

### Implementation
####TODO
