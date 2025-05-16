--TEST--
GH-18463: Recursion protection should not be applied to internal class constants
--EXTENSIONS--
zend_test
--FILE--
<?php

function handler($errno, $errstr, $errfile, $errline) {
	echo "$errstr in $errfile on line $errline\n";
	eval('class string {}');
}

set_error_handler('handler');

var_dump(_ZendTestClass::ZEND_TEST_DEPRECATED);
?>
--EXPECTF--
Constant _ZendTestClass::ZEND_TEST_DEPRECATED is deprecated in %s on line %d

Fatal error: Cannot use "string" as a class name as it is reserved in %s(%d) : eval()'d code on line %d
