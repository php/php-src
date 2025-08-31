--TEST--
OSS-Fuzz #439125710 (Pipe cannot be used in write context)
--FILE--
<?php
list(y|>y)=y;
?>
--EXPECTF--
Fatal error: Can't use function return value in write context in %s on line %d
