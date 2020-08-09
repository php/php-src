--TEST--
Test nullsafe in unset
--FILE--
<?php

$foo = null;
unset($foo?->bar->baz);

?>
--EXPECTF--
Fatal error: Can't use nullsafe operator in write context in %s.php on line 4
