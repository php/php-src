--TEST--
Test nullsafe property assignment op
--FILE--
<?php

$foo = null;
$foo?->bar += 1;

?>
--EXPECTF--
Fatal error: Can't use nullsafe operator in write context in %s.php on line 4
