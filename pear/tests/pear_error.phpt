--TEST--
PEAR_Error test
--SKIPIF--
--FILE--
<?php // -*- PHP -*-

// Test for: PEAR.php
// Parts tested: - PEAR_Error class
//               - PEAR::isError static method

require_once "../PEAR.php";

function test_error_handler($errno, $errmsg, $file, $line, $vars) {
	$errortype = array (
		1   =>  "Error",
		2   =>  "Warning",
		4   =>  "Parsing Error",
		8   =>  "Notice",
		16  =>  "Core Error",
		32  =>  "Core Warning",
		64  =>  "Compile Error",
		128 =>  "Compile Warning",
		256 =>  "User Error",
		512 =>  "User Warning",
		1024=>  "User Notice"
	);
	$prefix = $errortype[$errno];
	$file = basename($file);
	print "\n$prefix: $errmsg in $file on line $line\n";
}

error_reporting(E_ALL);
set_error_handler("test_error_handler");

class Foo_Error extends PEAR_Error
{
    function Foo_Error($message = "unknown error", $code = null,
                       $mode = null, $options = null, $userinfo = null)
    {
        $this->PEAR_Error($message, $code, $mode, $options, $userinfo);
        $this->error_message_prefix = 'Foo_Error prefix';
    }
}

class Test1 extends PEAR {
    function Test1() {
        $this->PEAR("Foo_Error");
    }
    function runtest() {
        return $this->raiseError("test error");
    }
}

function errorhandler(&$obj) {
    print "errorhandler function called, obj=".$obj->toString()."\n";
}

class errorclass {
    function errorhandler(&$obj) {
		print "errorhandler method called, obj=".$obj->toString()."\n";
    }
}

print "specify error class: ";
$obj = new Test1;
$err = $obj->runtest();
print $err->toString() . "\n";

$eo = new errorclass;

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

print "mode=callback(function): ";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_CALLBACK, "errorhandler");

print "mode=callback(method): ";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_CALLBACK,
                      array(&$eo, "errorhandler"));

print "mode=print&trigger: ";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_PRINT|PEAR_ERROR_TRIGGER);
print $err->toString() . "\n";

print "mode=trigger:";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_TRIGGER);
print $err->toString() . "\n";

print "mode=trigger,level=notice:";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_TRIGGER, E_USER_NOTICE);
print $err->toString() . "\n";

print "mode=trigger,level=warning:";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_TRIGGER, E_USER_WARNING);
print $err->toString() . "\n";

print "mode=trigger,level=error:";
$err = new PEAR_Error("test error", -42, PEAR_ERROR_TRIGGER, E_USER_ERROR);
print $err->toString() . "\n";

print "testing expectError:\n";
$obj =& new PEAR;
$obj->setErrorHandling(PEAR_ERROR_PRINT, "*** ERROR: %s\n");
print "expecting syntax/invalid\n";
$obj->expectError(array("syntax", "invalid"));
print "raising already_exists\n";
$err = $obj->raiseError("already_exists");
print "raising syntax\n";
$err = $obj->raiseError("syntax");
print "expecting syntax only\n";
$obj->expectError(array("syntax"));
print "raising invalid\n";
$err = $obj->raiseError("invalid");
print "popping\n";
var_dump($obj->popExpect());
print "raising invalid\n";
$err = $obj->raiseError("invalid");
print "popping\n";
var_dump($obj->popExpect());
print "raising invalid\n";
$err = $obj->raiseError("invalid");

?>
--GET--
--POST--
--EXPECT--
specify error class: [foo_error: message="test error" code=0 mode=return level=notice prefix="Foo_Error prefix" prepend="" append="" info=""]
default PEAR_Error: [pear_error: message="unknown error" code=0 mode=return level=notice prefix="" prepend="" append="" info=""]
Testing it: bool(true)
This is not an error: bool(false)
Now trying a bunch of variations...
different message: [pear_error: message="test error" code=0 mode=return level=notice prefix="" prepend="" append="" info=""]
different message,code: [pear_error: message="test error" code=-42 mode=return level=notice prefix="" prepend="" append="" info=""]
mode=print: test error[pear_error: message="test error" code=-42 mode=print level=notice prefix="" prepend="" append="" info=""]
mode=callback(function): errorhandler function called, obj=[pear_error: message="test error" code=-42 mode=callback callback=errorhandler prefix="" prepend="" append="" info=""]
mode=callback(method): errorhandler method called, obj=[pear_error: message="test error" code=-42 mode=callback callback=errorclass::errorhandler prefix="" prepend="" append="" info=""]
mode=print&trigger: test error
User Notice: test error in PEAR.php on line 650
[pear_error: message="test error" code=-42 mode=print|trigger level=notice prefix="" prepend="" append="" info=""]
mode=trigger:
User Notice: test error in PEAR.php on line 650
[pear_error: message="test error" code=-42 mode=trigger level=notice prefix="" prepend="" append="" info=""]
mode=trigger,level=notice:
User Notice: test error in PEAR.php on line 650
[pear_error: message="test error" code=-42 mode=trigger level=notice prefix="" prepend="" append="" info=""]
mode=trigger,level=warning:
User Warning: test error in PEAR.php on line 650
[pear_error: message="test error" code=-42 mode=trigger level=warning prefix="" prepend="" append="" info=""]
mode=trigger,level=error:
User Error: test error in PEAR.php on line 650
[pear_error: message="test error" code=-42 mode=trigger level=error prefix="" prepend="" append="" info=""]
testing expectError:
expecting syntax/invalid
raising already_exists
*** ERROR: already_exists
raising syntax
*** ERROR: syntax
expecting syntax only
raising invalid
*** ERROR: invalid
popping
array(1) {
  [0]=>
  string(6) "syntax"
}
raising invalid
*** ERROR: invalid
popping
array(2) {
  [0]=>
  string(6) "syntax"
  [1]=>
  string(7) "invalid"
}
raising invalid
*** ERROR: invalid
