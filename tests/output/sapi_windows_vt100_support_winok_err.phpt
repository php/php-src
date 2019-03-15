--TEST--
Test sapi_windows_vt100_support on newer Windows versions with redirected STDERR
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
STDERR
--FILE--
<?php
require __DIR__.'/sapi_windows_vt100_support.inc';
testToStdErr();
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
- enabling VT100 : bool(true)
- current value  : bool(true)
- disabling VT100: bool(true)
- current value  : bool(false)
STDOUT (fopen):
- current value  : bool(false)
- enabling VT100 : bool(true)
- current value  : bool(true)
- disabling VT100: bool(true)
- current value  : bool(false)
STDOUT (php://fd/1):
- current value  : bool(false)
- enabling VT100 : bool(true)
- current value  : bool(true)
- disabling VT100: bool(true)
- current value  : bool(false)
STDERR (constant):
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
STDERR (fopen):
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
STDERR (php://fd/2):
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
Not a stream:
- current value  : 
Warning: sapi_windows_vt100_support() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
- enabling VT100 : 
Warning: sapi_windows_vt100_support() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
- current value  : 
Warning: sapi_windows_vt100_support() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
- disabling VT100: 
Warning: sapi_windows_vt100_support() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
- current value  : 
Warning: sapi_windows_vt100_support() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
Invalid stream (php://temp):
- current value  : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- enabling VT100 : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- disabling VT100: 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
Invalid stream (php://input):
- current value  : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- enabling VT100 : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- disabling VT100: 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
Invalid stream (php://memory):
- current value  : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- enabling VT100 : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- disabling VT100: 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
- current value  : 
Warning: sapi_windows_vt100_support() was not able to analyze the specified stream in %s on line %d
bool(false)
File stream:
- current value  : bool(false)
- enabling VT100 : bool(false)
- current value  : bool(false)
- disabling VT100: bool(false)
- current value  : bool(false)
