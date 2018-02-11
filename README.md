This project is capable to be deployed on Raspberry Pi or Linux PC.

If you want to deploy it on Linux PC, make sure,
   1. gcc supports C++11.
   2. OpenCV 3.x installed.
   3. Built-in camera or USB camera connected.

If you want to deploy it on Raspberry Pi
   1. gcc supports C++11
   2. OpenCV 3.x installed.
   3. raspicam library installed
   	     https://github.com/cedricve/raspicam.git
   4. Camera connected via CSI port


Build on PC: make -j4
Build on Raspbarry Pi : make RPI=1 -j4