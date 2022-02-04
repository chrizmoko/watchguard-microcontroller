How to use "install.py".

The install.py script will take the third-party libraries required for the
project and install them in the Arduino library directory.


-- Requirements

* The Arduino IDE is required for communication with the microcontroller.

* Python3 is required to be installed on your machine to run the install.py
  script. This can be downloaded and installed from "https://www.python.org".


-- Instructions --

1. Open the "paths.json" file with a text editor. Replace the
   <arduino_library_path> with the path to your arduino libraries folder on your
   machine.

2. Open a terminal (or command prompt) and ensure the current working directory
   is the tools folder; this is the folder that contains this file along with
   the "install.py" script and "paths.json" file.

3. Call Python3 from the terminal and use it on the script. The command should
   be something similar to "python install.py". Note that you can simply
   double-click on the "install.py" file to coerce Python3 to run it, but if an
   error occurs, there is no course for understanding what happens.