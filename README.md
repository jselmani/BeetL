## BeetL

**Team Members:** [Jiel Selmani](https://www.linkedin.com/in/jielselmani/), [Eric Schvartzman](https://www.linkedin.com/in/eric-schvartzman-1a93a789/), [Rosario Cali](https://www.linkedin.com/in/rosario-alessandro-cal%C3%AC-b8480a128/)

**Project Description** 

A TCP/IP application written in C++ that was designed to interface with a robot and remotely control it.  The application provides commands that allow the Robot to move forward, backward, rotate to the right, rotate to the left, raise/lower the claw arm, and open/close the claw.  A near real-time bidirectional data channel is open due to multithreading, allowing to send commands and receive corresponding telemetry data back from the Robot itself.

Data is parsed into one byte values and stored into a dynamically allocated array containing the commands before being sent to the Robot.  Upon receipt of telemetry data, the buffer received is parsed into its original state and displayed in the command line.  This display is used to ensure a NACK was not received.

**Image of Robot**

![alt text](https://github.com/jselmani/Robotics-Data-Communications/blob/master/Robot.JPG)
