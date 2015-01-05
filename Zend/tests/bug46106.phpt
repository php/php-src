--TEST--
Bug #46106 (Memory leaks when using global statement)
--FILE--
<?php
$foo = array(1);

function foobar($errno, $errstr, $errfile, $errline) { }

set_error_handler('foobar');

function test($x) {
	global $foo;

	$x->invokeArgs(array(0));
}

$x = new ReflectionFunction('str_pad');
test($x);
?>
DONE
--EXPECT--
DONE
