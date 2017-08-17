--TEST--
Bug #73825 Heap out of bounds read on unserialize in finish_nested_data()
--FILE--
<?php
$obj = unserialize('O:8:"00000000":');
var_dump($obj);
?>
--EXPECTF--
Warning: Bad unserialize data in %sbug73825.php on line %d

Notice: unserialize(): Error at offset 13 of 15 bytes in %sbug73825.php on line %d
bool(false)
