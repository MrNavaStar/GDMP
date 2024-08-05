<img src="https://github.com/MrNavaStar/GDMP/blob/master/gdmp.png?raw=true" width="100%">

# GDMP

Get Down Mr President is an easy to use signal/io filtering application for unix! It can be run in conjuction with any other command.

# Usage

You can catch most unix signals sent to an application, excluding only special signals such as SIGKILL. You can find a full list of signals [here](https://www-uxsup.csx.cam.ac.uk/courses/moved.Building/signals.pdf).

A signal can be listened for by passing the name of the signal (excluding the sigpart):
```bash
# listen for SIGINT or ctr+c
```
You can block a signal from reaching the program by passing -x after the signal name.
```bash
# Block SIGINT or ctrl+c
gdmp -int -x my_prog --my-prog-flags/args
```
You can write to a programs stdin when a signal is captured. This works regaurdless if the signal is blocked or not.
```bash
# Write "hello program!" to stdin on SIGINT or ctrl+c
gdmp -int -x -w "hello program!" my_prog --my-prog-flags/args
```
You also run a bash command when a signal is captured.
```bash
gdmp -int -w -s "echo 'hello!'" my_prog --my-prog-flags/args
```

Signal chaining can be used to apply modifiers to signals in bulk:
```bash
# Block SIGINT, SIGTERM, SIGHUP and write "wow!" to stdin on SIGUSR1 or SIGUSR2.
gdmp -int -term -hup -x -usr1 -usr2 -w "wow!" my_prog --my-prog-flags/args
```

# Compiling and installing
You can download a prebuilt binary from the [releases page](https://github.com/MrNavaStar/GDMP/releases).

To compile the program for yourself you can run:
```bash
git clone https://github.com/MrNavaStar/GDMP.git
sudo make install
```
The program is a signal binary and will be installed to /usr/local/bin. The make file also provides `make uninstall` to remove the program for you.
