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
eval('abstract class foo { abstract static function bar(); }');

?>
--EXPECTF--
string(%d) "Static function foo::bar() should not be abstract"
string(%d) "%s(%d) : eval()'d code"

Notice: Undefined variable: undefined in %s on line %d
