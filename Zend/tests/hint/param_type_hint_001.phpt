--TEST--
Parameter type hint - Testing integer hint
--FILE--
<?php

function test_int($x, int $a = 1) {
}

test_int(1);
print "Ok\n";

test_int('1211');
print "Ok\n";

test_int(null);
print "Ok\n";

test_int(1, 1);
print "Ok\n";

test_int(null, '1211');
print "Ok\n";

?>
--EXPECTF--
Ok
Ok
Ok
Ok

Catchable fatal error: Argument 2 passed to test_int() must be of the type integer, string given, called in %s on line %d and defined in %s on line %d
--UEXPECTF--
Ok
Ok
Ok
Ok

Catchable fatal error: Argument 2 passed to test_int() must be of the type integer, Unicode string given, called in %s on line %d and defined in %s on line %d
