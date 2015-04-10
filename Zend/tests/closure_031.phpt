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
try {
	var_dump($foo->a);
} catch (EngineException $ex) {
	echo "Error: {$ex->getMessage()}\n";
}
?>
--EXPECT--
Error: Closure object cannot have properties

