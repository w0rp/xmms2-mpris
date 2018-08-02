# xmms2-mpris

A client for XMMS2 which displays current track information with MPRIS, and
implements MPRIS controls for controlling XMMS2.

## Building on Ubuntu/Linux Mint/KDE Neon

Install the following things first.

```
sudo apt install cmake libxmmsclient-dev libglib2.0-dev
```

Then build the project with CMake.

```
cmake .
make
```

You can run the client with `./xmms2-mpris`. The `xmms2d` will need to already
be running.
