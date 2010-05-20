--TEST--
Parameter type hint - Testing default parameter values
--FILE--
<?php

function test(string $a, bool $b = NULL, integer $c, double $d = NULL, $e) {
	print "ok\n";
}

test('foo', NULL, 1, NULL, 'bar');
test('foo', true, 0, -1., NULL);
test('true', false, -1, 2.2222, 1.1);
test('1.1', false, -1, 11111111111111111111111111, true);
test('1', false, -1, NULL, new stdClass);

?>
--EXPECT--
ok
ok
ok
ok
ok
