# Cpp-image-alignment-2dcamera
C++ OpenCV project to align an object with a 2D camera based on image data

### Building the Project
To build this project, first clone the repo.
```bash
mkdir output
mkdir build && cd build
cmake ..
make
./AlignImage -l
```

### Purpose
The purpose of this tool will be to allow the user, through repeated use of the tool, to take an image of an object and move it until it aligns horizontally and vertically with the edges of a camera. 

This is a known issue on production lines (particularly displays) where the accuracy of camera measurements depends on camera alignment.

The aim of the project is to read in images of an object and tell the user whether the camera is square with the object.
Conceptually, this is similar to a spirit-level, it will tell the user if an object is tilted to left, right, or up or down.
An end-view or side-view of the object is needed, where the program will get the 4 corners of the object (in this case a LCD display) and output the different between x-plane and the y-plane of each corner. With a rectangular object, this means that the left (top-left and bottom-left) corners are on the same plane, similar the right (top-right and bottom-right) corners are on the same plane and the same for the top (top-left and top-right) corners, etc.

### Overview
At a high-level, the user will use this tool by taking an image with a camera, to create an image file (like a JPEG, or PNG). They will then place the file in a folder, and specify the folder in the config.json file of this program. By running the program, they will see if the object in the image lines up correctly with the camera. The degree to which the top and left edges of the object line up with the top and left edges of the camera will be given to the user.

***In order to validate this***, there is are several image included in the ```validationImages/``` folder. Follow the build instructions, run the program and check the ```output/``` folder. The output image will show the contours and corners of the image. See the log file (the path is specified in the config.json) to read the horizontal and vertical anglular alignment.

### Requirements
This project is the Capstone submission for the Udacity nanodegree program, which requires several features:
* Demonstates an understanding of C++ functions and control structures
* File IO
* Well design OOP incorporating abstraction, encapsulation and inheritance.
* Use of proper memory management - references, RAII, move semantics and smart pointers.
* Use of concurrent programming - mutlithreading is utilized safely through promises and futures, mutexes and locks.

### Features & Usage
The intent of the program is to read in an image, process it, overlay lines over the edges of an object and output the resulting image. It should also use the co-ordiniates of the top-left, top-right, bottom-left and bottom-right corners of the image to calculate the degree to which the respective planes differ in degrees.
The input, then, will be image files and the output will be image files and the horizontal and vertical alignment (in degrees) printed in the log file.

### Implementation
This project will fulfill it's requirements the following way:
#### OOP 
There will be several classes used, ImageData (for reading, cropping and writing the images), Logger (generic logger class to assist in debug), MessageQueue (to allow for concurrent programming) and ImageDataPtr (for memory management).
#### File IO
Input will be taken and parsed from a config file and output will be sent to a log file.
#### Memory Management
The progression of each stage of the image processing will be controlled using shared pointers, ensuring the deletion of the memory at the end of the program.
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
