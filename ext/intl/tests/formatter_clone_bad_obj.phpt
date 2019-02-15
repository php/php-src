--TEST--
Cloning unconstructed numfmt
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

class A extends NumberFormatter {
	function __construct() {}
}

$a = new A;
try {
	$b = clone $a;
} catch (Exception $e) {
	var_dump($e->getMessage());
}
--EXPECT--
string(42) "Cannot clone unconstructed NumberFormatter"
