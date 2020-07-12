# xmms2-mpris

A client for XMMS2 which displays current track information with MPRIS, and
implements MPRIS controls for controlling XMMS2.

This allows the system to show information and controls for media that is playing. KDE will show the track and album art in your system tray and lock screen.

![system-tray-example](https://user-images.githubusercontent.com/3518142/87245779-2b33fd80-c440-11ea-9260-80dfe58d6a3f.jpg)

## Building on Ubuntu/Linux Mint/KDE Neon

Install the following things first.

```
sudo apt install cmake libxmmsclient-dev libglib2.0-dev libxmmsclient-glib-dev
```

Then build the project with CMake.

```
cmake .
make
```

You can run the client with `./xmms2-mpris`. The `xmms2d` will need to already
be running. You can run the install script to install the client to
`~/.config/xmms2/startup.d/`, so it will automatically start whenever xmms2
does.

```
make install
```

You shouldn't install the client as `root` or another user. Install it as
the same user that's starting `xmms2d`.
