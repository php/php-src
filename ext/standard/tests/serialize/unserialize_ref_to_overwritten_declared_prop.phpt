--TEST--
Trying to create a reference to an overwritten declared property
--FILE--
<?php
$str = <<<STR
O:9:"Attribute":1:{s:5:"flags";i:63;s:5:"flags";R:2}
STR;
var_dump(unserialize($str));
?>
--EXPECTF--
Warning: unserialize(): Error at offset 36 of 52 bytes in %s on line %d
bool(false)
