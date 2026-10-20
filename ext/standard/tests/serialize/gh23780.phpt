--TEST--
GH-23780: unserialize('') fails silently
--FILE--
<?php
var_dump(unserialize(''));
?>
--EXPECTF--
Warning: unserialize(): Error at offset 0 of 0 bytes in %s on line %d
bool(false)
