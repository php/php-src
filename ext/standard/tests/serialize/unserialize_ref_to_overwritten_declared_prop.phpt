--TEST--
Trying to create a reference to an overwritten declared property
--FILE--
<?php
$str = <<<STR
O:5:"Error":2:{S:8:"previous";N;S:8:"previous";R:2;}
STR;
var_dump(unserialize($str));
?>
--EXPECTF--
Notice: unserialize(): Error at offset 51 of 52 bytes in %s on line %d
bool(false)
