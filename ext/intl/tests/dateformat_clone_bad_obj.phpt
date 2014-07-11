--TEST--
Cloning unconstructed IntlDateFormatter
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

class A extends IntlDateFormatter {
	function __construct() {}
}

$a = new A;
try {
	$b = clone $a;
} catch (Exception $e) {
	var_dump($e->getMessage());
}

--EXPECTF--
string(%s) "Cannot clone unconstructed IntlDateFormatter"
