--TEST--
PEAR_Error: basic test
--SKIPIF--
skip
--FILE--
<?php // -*- PHP -*-

// Test for: PEAR.php
// Parts tested: - PEAR_Error class
//               - PEAR::isError static method

include dirname(__FILE__)."/../PEAR.php";

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
	if (preg_match('/^The call_user_method.. function is deprecated/',
	    $errmsg)) {
	    return;
	}
	$prefix = $errortype[$errno];
	$file = basename($file);
	print "\n$prefix: $errmsg in $file on line XXX\n";
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

?>
--GET--
--POST--
--EXPECT--
specify error class: [foo_error: message="test error" code=0 mode=return level=notice prefix="Foo_Error prefix" info=""]
default PEAR_Error: [pear_error: message="unknown error" code=0 mode=return level=notice prefix="" info=""]
Testing it: bool(true)
This is not an error: bool(false)
Now trying a bunch of variations...
different message: [pear_error: message="test error" code=0 mode=return level=notice prefix="" info=""]
different message,code: [pear_error: message="test error" code=-42 mode=return level=notice prefix="" info=""]
mode=print: test error[pear_error: message="test error" code=-42 mode=print level=notice prefix="" info=""]
mode=callback(function): errorhandler function called, obj=[pear_error: message="test error" code=-42 mode=callback callback=errorhandler prefix="" info=""]
mode=callback(method): errorhandler method called, obj=[pear_error: message="test error" code=-42 mode=callback callback=errorclass::errorhandler prefix="" info=""]
mode=print&trigger: test error
User Notice: test error in PEAR.php on line XXX
[pear_error: message="test error" code=-42 mode=print|trigger level=notice prefix="" info=""]
mode=trigger:
User Notice: test error in PEAR.php on line XXX
[pear_error: message="test error" code=-42 mode=trigger level=notice prefix="" info=""]
mode=trigger,level=notice:
User Notice: test error in PEAR.php on line XXX
[pear_error: message="test error" code=-42 mode=trigger level=notice prefix="" info=""]
mode=trigger,level=warning:
User Warning: test error in PEAR.php on line XXX
[pear_error: message="test error" code=-42 mode=trigger level=warning prefix="" info=""]
mode=trigger,level=error:
User Error: test error in PEAR.php on line XXX
[pear_error: message="test error" code=-42 mode=trigger level=error prefix="" info=""]
