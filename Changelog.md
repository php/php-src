ChangeLog for phpdbg
====================

Version 0.1.0 2013-11-23
------------------------

1. New commands:
  - until  (continue until the current line is executed)
  - frame  (switch to a frame in the current stack for inspection)
  - info   (quick access to useful information on the console)
  - finish (continue until the current function has returned)
  - leave  (continue until the current function is returning)
  - shell  (shell a command)
2. Added printers for class and method
3. Make uniform commands and aliases where possible
4. Include all alias information and sub-command information in help
5. Added signal handling to break execution (ctrl-c)


Version 0.0.1 2013-11-15
------------------------

1. Initial features
