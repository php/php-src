--TEST--
Bug #25378 (unserialize() crashes with invalid data)
--FILE--
<?php 
var_dump(unserialize("s:-1:\"\";"));
?>
--EXPECTF--
Notice: unserialize(): Error at offset 0 of 8 bytes in %s on line %d
bool(false)
