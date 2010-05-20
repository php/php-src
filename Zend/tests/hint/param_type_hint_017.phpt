--TEST--
Parameter type hint - Testing default parameter values as constants
--FILE--
<?php

function test(int $foo = PHP_INT_MAX) {
	print "ok\n";
}

test();

?>
--EXPECT--
ok
