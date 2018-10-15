--TEST--
Cloning unconstructed MessageFormatter
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

class A extends MessageFormatter {
	function __construct() {}
}

$a = new A;
try {
	$b = clone $a;
} catch (Exception $e) {
	var_dump($e->getMessage());
}
--EXPECTF--
string(%d) "Cannot clone unconstructed MessageFormatter"
