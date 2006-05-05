--TEST--
Bug #37251 (deadlock when custom error handler is to catch array type hint error) 
--FILE--
<?php
function error_handler($errno, $errstr, $errfile, $errline, $context) {
	echo 'OK';
}

set_error_handler('error_handler');

class Foo {	
	function bar(array $foo) {
	}
}

$foo = new Foo();
$foo->bar();
--EXPECT--
OK
