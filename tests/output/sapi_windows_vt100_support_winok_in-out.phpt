--TEST--
Test sapi_windows_vt100_support on newer Windows versions with redirected STDIN/STDOUT
--SKIPIF--
<?php
if (getenv("SKIP_IO_CAPTURE_TESTS")) {
    die("skip I/O capture test");
}
if(PHP_OS_FAMILY !== "Windows") {
    echo "skip Only for Windows systems";
} elseif (version_compare(
    PHP_WINDOWS_VERSION_MAJOR.'.'.PHP_WINDOWS_VERSION_MINOR.'.'.PHP_WINDOWS_VERSION_BUILD,
     '10.0.10586'
) < 0) {
    echo "skip Only for Windows systems >= 10.0.10586";
}
?>
--CAPTURE_STDIO--
STDIN STDOUT
--FILE--
<?php
require __DIR__.'/sapi_windows_vt100_support.inc';
testToStdOut();
?>
--EXPECTF--
STDIN (constant):
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
STDIN (fopen):
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
STDIN (php://fd/0):
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
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
- current value  : bool(false)
- enabling VT100 : bool(true)
- current value  : bool(true)
- disabling VT100: bool(true)
- current value  : bool(false)
STDERR (fopen):
- current value  : bool(false)
- enabling VT100 : bool(true)
- current value  : bool(true)
- disabling VT100: bool(true)
- current value  : bool(false)
STDERR (php://fd/2):
- current value  : bool(false)
- enabling VT100 : bool(true)
- current value  : bool(true)
- disabling VT100: bool(true)
- current value  : bool(false)
Invalid stream (php://temp):
- current value  : bool(false)
- enabling VT100 : 
Warning: sapi_windows_vt100_support(): not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : bool(false)
- disabling VT100: 
Warning: sapi_windows_vt100_support(): not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : bool(false)
Invalid stream (php://input):
- current value  : bool(false)
- enabling VT100 : 
Warning: sapi_windows_vt100_support(): not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : bool(false)
- disabling VT100: 
Warning: sapi_windows_vt100_support(): not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : bool(false)
Invalid stream (php://memory):
- current value  : bool(false)
- enabling VT100 : 
Warning: sapi_windows_vt100_support(): not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : bool(false)
- disabling VT100: 
Warning: sapi_windows_vt100_support(): not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : bool(false)
File stream:
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
