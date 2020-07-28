--TEST--
Refcount of constant LHS with nullsafe operator
--FILE--
<?php
['']?->a;
?>
--EXPECTF--
Warning: Attempt to read property "a" on array in %s on line %d
