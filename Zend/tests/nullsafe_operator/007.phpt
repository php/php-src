--TEST--
Test nullsafe property pre increment
--FILE--
<?php

$foo = null;
var_dump($foo?->bar++);

?>
--EXPECTF--
Fatal error: Can't use nullsafe operator in write context in %s.php on line 4
