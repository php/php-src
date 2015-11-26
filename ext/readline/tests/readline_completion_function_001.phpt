--TEST--
readline_completion_function(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline")) die("skip"); ?>
--FILE--
<?php

function foo() { }

$data = array(
	'foo',
	'strtolower',
	1,
	1.1231
);

foreach ($data as $callback) {
	readline_completion_function($callback);
}

?>
--EXPECTF--
Warning: readline_completion_function(): 1 is not callable in %s on line %d

Warning: readline_completion_function(): 1.1231 is not callable in %s on line %d
