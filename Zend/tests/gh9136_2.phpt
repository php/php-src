--TEST--
GH-9136: Assertion when fetching property of magic constant in constant expression with nullsafe operator
--FILE--
<?php

const C = __file__?->foo;

?>
--EXPECTF--
Warning: Attempt to read property "foo" on string in %s on line %d
