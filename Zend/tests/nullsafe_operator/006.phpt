--TEST--
Test nullsafe property post increment
--FILE--
<?php

$foo = null;
++$foo?->bar;

?>
--EXPECTF--
Fatal error: Can't use nullsafe operator in write context in %s.php on line 4
