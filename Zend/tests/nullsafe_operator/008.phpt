--TEST--
Test nullsafe property coalesce assignment
--FILE--
<?php

$foo = null;
var_dump($foo?->bar ??= 'bar');

?>
--EXPECTF--
Fatal error: Can't use nullsafe operator in write context in %s.php on line 4
