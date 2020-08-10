--TEST--
Refcount of constant LHS with nullsafe operator
--FILE--
<?php
['']?->a;
__DIR__?->a;
?>
--EXPECTF--
Warning: Attempt to read property "a" on array in %s on line %d

Warning: Attempt to read property "a" on string in %s on line %d
