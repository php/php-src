--TEST--
Bug #71840 (Unserialize accepts wrongly data)
--FILE--
<?php
var_dump(unserialize('a:1:{s:0:""0a:0:{}}'));
?>
--EXPECTF--
Notice: unserialize(): Error at offset 11 of 19 bytes in %sbug71840.php on line %d
bool(false)
