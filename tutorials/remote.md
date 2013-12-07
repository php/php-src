Remote Debugging
================
*Using phpdbg across the network*

It is sometimes useful to execute some code in the environment that exhibits some bugs; while there is a pretty good chance of being able to mock the environment that exhibited the bug on another machine, it is sometimes impossible or impractical.

Remote debugging allows you to start an instance of phpdbg on any machine, and connect to it from any machine, bundled is a remote client implemented in Java:

![phpdbg jui](https://raw.github.com/krakjoe/phpdbg/master/tutorials/java-example.png)


Starting phpdbg in Remote Console Mode
======================================
*Starting the server ...*

Simply open a terminal on the target machine and issue the command

```
./phpdbg -l4000
```

This will initialize phpdbg in Remote Console Mode, using port 4000 for stdin and port 8000 for stdout.

To stop the service, send *SIGINT* to the phpdbg process, this will force it to bailout, as gracefully as it ever does.

Specific Ports
--------------

To listen on specific ports, the -l flag will accept a pair of ports in the following way:

```
./phpdbg -l4000/8000
```

Specific Interfaces
-------------------

To bind to a specific interface, or all interfaces use the -a flag:

```
./phpdbg -l4000 -a192.168.0.3
```

Will bind the Remote Console to the interface with the specified address, while:

```
./phpdbg -l4000 -a
```

Will bind the Remote Console to all available addresses.

Starting phpdbg-jui Anywhere
============================
*Java is everywhere, so is phpdbg ...*

A JRE is needed for the bundled remote client, given any operating system with a working installation of Java:

```
java -jar /path/to/phpdbg-ui.jar
```

Will initialize the bundled client, simply configure the settings accordingly and press *Connect*

If disconnection is normal, the environment persists - another team member can pick up the session where it is left off.

Debugging remotely is no different to debugging locally, all of the same functionality is provided remotely.
