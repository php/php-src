--TEST--
PEAR_Error in callback mode
--SKIPIF--
--FILE--
<?php

require_once "PEAR.php";

function error_function($obj) {
    print "this is error_function reporting: ";
    print $obj->toString();
    print "\n";
}
class myclass {
    function error_method($obj) {
	print "this is myclass::error_method reporting: ";
	print $obj->toString();
	print "\n";
    }
}
$obj = new myclass;
new PEAR_Error("errortest1", 0, PEAR_ERROR_CALLBACK, "error_function");
new PEAR_Error("errortest2", 0, PEAR_ERROR_CALLBACK,
	       array(&$obj, "error_method"));


?>
--GET--
--POST--
--EXPECT--
this is error_function reporting: [pear_error: message="errortest1" code=0 mode=callback level=notice prefix="" prepend="" append=""]
this is myclass::error_method reporting: [pear_error: message="errortest2" code=0 mode=callback level=notice prefix="" prepend="" append=""]
