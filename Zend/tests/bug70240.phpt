--TEST--
Bug #70240 (Segfault when doing unset($var()))
--FILE--
<?php
unset($var());
?>
--EXPECTF--
Fatal error: Cannot use function return value in write context in %s on line %d
