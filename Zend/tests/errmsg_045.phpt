--TEST--
Error message in error handler during compilation
--FILE--
<?php

set_error_handler(function($_, $msg, $file) {
	var_dump($msg, $file);
	echo $undefined;
});

/* This is just a particular example of a non-fatal compile-time error
 * If this breaks in future, just find another example and use it instead */
eval('class A { private function __invoke() { } }');

?>
--EXPECTF--
string(76) "The magic method __invoke() must have public visibility and cannot be static"
string(%d) "%s(%d) : eval()'d code"

Warning: Undefined variable: undefined in %s on line %d
