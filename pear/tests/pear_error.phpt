--TEST--
PEAR_Error test
--SKIPIF--
--FILE--
<?php // -*- C++ -*-

// Test for: PEAR.php
// Parts tested: - PEAR_Error class
//               - PEAR::isError static method
// testing PEAR_Error

require_once "PEAR.php";

error_reporting(4095);

print "default PEAR_Error: ";
$err = new PEAR_Error;
print $err->toString() . "\n";
print "Testing it: ";
var_dump(PEAR::isError($err));
print "This is not an error: ";
$str = "not an error";
var_dump(PEAR::isError($str));

print "Now trying a bunch of variations...\n";
print "different message: ";
$err = new PEAR_Error("test error");
print $err->toString() . "\n";
print "different message,code: ";
$err = new PEAR_Error("test error", -42);
print $err->toString() . "\n";
print "mode=print: ";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_PRINT);
print $err->toString() . "\n";
print "mode=trigger: ";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_TRIGGER);
print $err->toString() . "\n";
print "mode=trigger,level=notice: ";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_TRIGGER, E_USER_NOTICE);
print $err->toString() . "\n";
print "mode=trigger,level=warning: ";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_TRIGGER, E_USER_WARNING);
print $err->toString() . "\n";
print "mode=trigger,level=error: ";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_TRIGGER, E_USER_ERROR);
print $err->toString() . "\n";

?>
--GET--
--POST--
--EXPECT--
default PEAR_Error: [pear_error: message="unknown error" code=0 mode=return level=notice prefix="" prepend="" append=""]
Testing it: bool(true)
This is not an error: bool(false)
Now trying a bunch of variations...
different message: [pear_error: message="test error" code=0 mode=return level=notice prefix="" prepend="" append=""]
different message,code: [pear_error: message="test error" code=-42 mode=return level=notice prefix="" prepend="" append=""]
mode=print: test error[pear_error: message="test error" code=-42 mode=print level=notice prefix="" prepend="" append=""]
mode=trigger: <br>
<b>Notice</b>:  test error in <b>PEAR.php</b> on line <b>198</b><br>
[pear_error: message="test error" code=-42 mode=trigger level=notice prefix="" prepend="" append=""]
mode=trigger,level=notice: <br>
<b>Notice</b>:  test error in <b>PEAR.php</b> on line <b>198</b><br>
[pear_error: message="test error" code=-42 mode=trigger level=notice prefix="" prepend="" append=""]
mode=trigger,level=warning: <br>
<b>Warning</b>:  test error in <b>PEAR.php</b> on line <b>198</b><br>
[pear_error: message="test error" code=-42 mode=trigger level=warning prefix="" prepend="" append=""]
mode=trigger,level=error: <br>
<b>Fatal error</b>:  test error in <b>PEAR.php</b> on line <b>198</b><br>
