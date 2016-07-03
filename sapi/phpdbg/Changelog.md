ChangeLog for phpdbg
====================

Version 0.3.0 2013-00-00
------------------------

1. Added ability to disable an enable a single breakpoint
2. Added ability to override SAPI name
3. Added extended conditional breakpoint support "break at"
4. Fix loading of zend extnsions with -z
5. Fix crash when loading .phpdbginit with command line switch
6. Fix crash on startup errors
7. Added init.d for remote console (redhat)
8. Added phpdbg_exec userland function
9. Added testing facilities
10. Added break on n-th opline support
11. Improved trace output

Version 0.2.0 2013-11-31
------------------------

1. Added "break delete <id>" command
2. Added "break opcode <opcode>" command
3. Added "set" command - control prompt and console colors
4. .phpdbginit now searched in (additional) ini dirs
5. Added source command - load additional .phpdbginit script during session
6. Added remote console mode
7. Added info memory command

Version 0.1.0 2013-11-23
------------------------

1. New commands:
  - until    (continue until the current line is executed)
  - frame    (switch to a frame in the current stack for inspection)
  - info     (quick access to useful information on the console)
  - finish   (continue until the current function has returned)
  - leave    (continue until the current function is returning)
  - shell    (shell a command)
  - register (register a function for use as a command)
2. Added printers for class and method
3. Make uniform commands and aliases where possible
4. Include all alias information and sub-command information in help
5. Added signal handling to break execution (ctrl-c)
6. Fixed #13 (Output Buffering Control seems fail)
7. Fixed #14 (Fixed typo in Makefile.frag)


Version 0.0.1 2013-11-15
------------------------

1. Initial features
