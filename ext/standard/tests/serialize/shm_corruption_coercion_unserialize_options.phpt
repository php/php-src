--TEST--
Shm corruption with coercion in options of unserialize()
--FILE--
<?php
unserialize("{}", ["allowed_classes" => [0]]);
?>
--EXPECTF--
Warning: unserialize(): Error at offset 0 of 2 bytes in %s on line %d
