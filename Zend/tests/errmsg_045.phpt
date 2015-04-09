--TEST--
Error message in error handler during compilation
--FILE--
<?php

set_error_handler(function($_, $msg, $file) {
	var_dump($msg, $file);
	echo $undefined;
});

eval('class A { function a() {} function __construct() {} }');

?>
--EXPECTF--
string(50) "Redefining already defined constructor for class A"
string(%d) "%s(%d) : eval()'d code"

Notice: Undefined variable: undefined in %s on line %d
