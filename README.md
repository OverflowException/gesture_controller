This is an OpenCV-based motion sensing game controller.
It is capable to be deployed on Raspberry Pi or Linux PC.

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


Usage:
Under ./bin directory,
1. Change the IP address and port number in
       connection_config
   to server IP and port number you want to connect to. This is called "connection configuration" file.

2. Caliberate reference histogram
      ./caliberate [histogram data file name]
  Cover all the rectangles with your palm, follow the instructions on window title.
  Caliberated histogram data will be stored in [histogram data file name], to be used later.

3. Make sure server is alreading waiting to accept. A demo of the server
      https://github.com/OverflowException/baymaxmatch.git
   Then run the controller
      ./controller [connection configuration file name] [histogram data file name]

note:
When running controller, make sure your hands and face are the only skin-colored objects in sight.
Especially, when naked, or with sleeves rolled up, this controller is completely unusable.