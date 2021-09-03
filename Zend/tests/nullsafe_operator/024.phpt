--TEST--
Test nullsafe as foreach target
--FILE--
<?php

$foo = null;
foreach ([1, 2, 3] as $foo?->bar) {}

?>
--EXPECTF--
Fatal error: Can't use nullsafe operator in write context in %s.php on line 4
