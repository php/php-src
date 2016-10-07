--TEST--
Test stream_vt100_support on POSIX with redirected STDOUT
--SKIPIF--
<?php
if (!function_exists('posix_isatty')) {
    echo "skip Only for POSIX systems";
}
?>
--CAPTURE_STDIO--
STDOUT
--FILE--
<?php
require 'vt100_support.php';
testVT100ToStdOut();
?>
--EXPECTF--
STDIN (constant):
- current value  : bool(true)
- enabling VT100 : bool(false)
- current value  : bool(true)
- disabling VT100: bool(false)
- current value  : bool(true)
STDIN (fopen):
- current value  : bool(true)
- enabling VT100 : bool(false)
- current value  : bool(true)
- disabling VT100: bool(false)
- current value  : bool(true)
STDIN (php://fd/0):
- current value  : bool(true)
- enabling VT100 : bool(false)
- current value  : bool(true)
- disabling VT100: bool(false)
- current value  : bool(true)
STDOUT (constant):
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
STDOUT (fopen):
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
STDOUT (php://fd/1):
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
STDERR (constant):
- current value  : bool(true)
- enabling VT100 : bool(false)
- current value  : bool(true)
- disabling VT100: bool(false)
- current value  : bool(true)
STDERR (fopen):
- current value  : bool(true)
- enabling VT100 : bool(false)
- current value  : bool(true)
- disabling VT100: bool(false)
- current value  : bool(true)
STDERR (php://fd/2):
- current value  : bool(true)
- enabling VT100 : bool(false)
- current value  : bool(true)
- disabling VT100: bool(false)
- current value  : bool(true)
Not a stream:
- current value  : 
Warning: stream_vt100_support() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
- enabling VT100 : 
Warning: stream_vt100_support() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
- current value  : 
Warning: stream_vt100_support() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
- disabling VT100: 
Warning: stream_vt100_support() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
- current value  : 
Warning: stream_vt100_support() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
Invalid stream (php://temp):
- current value  : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- enabling VT100 : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- disabling VT100: 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
Invalid stream (php://input):
- current value  : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- enabling VT100 : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- disabling VT100: 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
Invalid stream (php://memory):
- current value  : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- enabling VT100 : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- disabling VT100: 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: stream_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
File stream:
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
