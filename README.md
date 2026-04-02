

# devicegrabber

devicegrabber is an application that will intercept the output from any input
device found in `/dev/input/` with a filename of `event` followed by a number.


## Dependencies

-   libevdev
-   systemd


## Installation

    git clone https://github.com/pojomi/devicegrabber.git

From repo root directory:

    make install


## Usage

To try it out after install, run the command

    sudo devicegrabber

A list of available devices will appear. Make a selection by entering the number
at the beginning of the line.

You will then be prompted a second time. Enter `0` to exit, `1` to immediately
grab the selected device, or `2` to create a systemd daemon for the selected device.


### Option 1

Immediately grab the selected device; intercepting all output immediately. Use your
terminal defined command to kill the process (commonly `C-c`).


### Option 2

Create a systemd unit file for the selected device. This will be a standard `.service`
file located in `/etc/systemd/system/`. It will **automatically** be enabled and started.
After selecting Option 2, the last line printed will provide the full name of the file
generated. General format is `devicegrabber-event<number>.service`.

To disable:

    sudo systemctl disable devicegrabber-event<number>.service &&
    sudo killall devicegrabber

