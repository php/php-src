--TEST--
Parameter type hint - Testing NULL when allowed
--FILE--
<?php

function test_int(int $a = null) {
}
test_int(null);

function test_double(double $a = null) {
}
test_double(null);

function test_bool(bool $a = null) {
}
test_bool(null);

print "Done";

?>
--EXPECT--
Done
