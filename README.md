# Autostart

Project to restart a UPS3v2 board (and the attached Raspberry PI) after a lightning induced shutdown.

We love living in central Florida, but do travel extensively (before the pandemic that is). After a season of numerous and serious lightning storms here in central Florida, many occurring while I was traveling away from home, a Lightning Detection system was implmented here.  The system rates the potential of nearby lightning strikes, and will publish that information to any RPi computer programmed to check.  Once the probability gets to 100%, the RPi computers will command their controlling UPS controllers to "shut me down, turn me off, and unplug me from the wall".

Normally, one must be present to press the UPS "Start" pushbutton to bring everything back up.  But, built into the Disconnector project is an ATTiny that is optically isolated from the UPS board.  It is powered on when the UPS and the RPi are powered off, running from a pair of alkaline AA-sized cells.  After a period of time (generally one hour), the ATTiny will electrically press the start button, and assuming wall power is available, every thing will start back up. 

Blog description:
https://gswann.blogspot.com/2019/07/auto-start.html

