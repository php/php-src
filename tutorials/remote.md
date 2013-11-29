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

*To get help using these options see the help menu in phpdbg "help options"*


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
