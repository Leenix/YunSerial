Adapted from https://gist.github.com/wayoda/db3c023417757f726088

# A bit about the Yun's Bridge

The Arduino Yun combines an Arduino microcontroller (ATmega32u4) with a Linux processor (OpenWRT AR9331) to provide a flexible and powerful interface for low-level IO combined with WiFi and powerful data processing. Serial communication is used to transfer information between the two cores, which is managed by a Bridge class that runs linux-side. However, for many purposes, the Bridge is not fast enough to process large amounts of data, and must be bypassed to run custom communication protocols.

    Serial1 (Arduino) <====> /dev/ttyATH0 (Yun)

The following steps will guide you to disable the Yun's bridging functionality to allow custom serial functionality.

# Connect to the Yun

To configure the Yun, we'll need to get access to it's command line terminal. The easiest way to do this is to use SSH.
If you're not familiar with SSH, [follow this link for some more information](https://www.arduino.cc/en/Tutorial/LinuxCLI).

Then, using your preferred SSH client or method, connect the Yun to your network using an Ethernet cable or WiFi (or connect to the Yun's WiFi hotspot). By default, you'll find the Yun at `192.168.240.1` or `arduino.local` on your network.

The default login for the Yun is:

    Username: root
    Password: arduino

# Stop the Bridge shell

The Bridge shell starts after the Yun has booted. You can prevent this shell from starting by commenting out its entry in the Yun's initialization table.

Open the `/etc/inittab` file in your preferred editor.

    nano /etc/inittab

Now insert a '#' before the last line to disable the bridge.

    ::sysinit:/etc/init.d/rcS S boot
    ::shutdown:/etc/init.d/rcS K shutdown
    # Comment out the following to disable the Yun bridge and allow fast serial
    # communication with custom code.
    #ttyATH0::askfirst:/bin/ash --login

Changes will take effect upon next restart. The bridge serial port can now be accessed through Python (or other) by opening /dev/ttyATH0.

# Allow Arduino handshaking

Now that you can use the serial bridge to its full potential, you'll need to be a bit careful. Without the Bridge class to manage the link, the Arduino has access to a full Linux shell. The microcontroller sending messages at the wrong time can interrupt the Yun's boot process. Thankfully, the Yun and Arduino microcontroller can use the handshake pin (PIN 7 on the Arduino).

The basic idea is that serial transmissions from the Arduino side are suppressed until the Yun has successfully booted. Installing the `boot-handshake` script causes the handshake pin to be driven LOW when the Yun is ready, and HIGH when the Yun is booting or reset.

## `/etc/init.d/boot-handshake`


1) Copy boot-handshake to the Yun's /etc/init.d/

2) In commandline, type the following to make the file executable

    chmod a+x /etc/init.d/boot-handshake

3) Install the script

    /etc/init.d/boot-handshake enable

4) You can test the script with the following commands:

    /etc/init.d/boot-handshake start
    /etc/init.d/boot-handshake stop


## Using the handshake

That just about does it for the Linux side. Now the Arduino side needs to use the handshaking signal to decide if its safe to transmit data across the serial bridge. Use the example code provided in `YunSerial.ino` to monitor the handshake pin using hardware interrupts.
