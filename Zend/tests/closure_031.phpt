--TEST--
Closure 031: Closure properties with custom error handlers
--FILE--
<?php
function foo($errno, $errstr, $errfile, $errline) {
	echo "Error: $errstr\n";
}
set_error_handler('foo');
$foo = function() {
};
var_dump($foo->a);
?>
--EXPECT--
Error: Closure object cannot have properties
NULL

