# avr-jaguar

This is an Arduino based test fixture for the Jaguar motor controller. The
objective is allow joystick control of a motor during the fabrication and test
of an FRC robot.


# Hardware

The test fixture requires the following hardware:
* Arduino microcontroller
* Joystick Shield v2.4
* Jaguar Motor Controller
* 12/24VDC battery
* 12/24 DC motor

Refer to data sheets for wiring.


# Software Installation

This program is complied and run using gcc instead of the Arduino IDE. The
following instructions work on Debian. 

```bash
# Install Arduino and avr tools
sudo apt install arduino
sudo apt install gcc-avr avr-libc binutils-avr avrdud
sudo apt install screen

# Clone the project
git clone git@github.com:rogermoore6872/avr-jaguar.git
cd avr-jaguar

# Select your board type name from the available options
make boards

# Configure the board type in .bashrc. Substitute your board type.
echo "export BOARD=uno" >> ~/.bashrc
source ~/.bashrc

# Build the program and run it
make all upload
```

