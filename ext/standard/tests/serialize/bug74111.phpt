--TEST--
Bug #74111: Heap buffer overread (READ: 1) finish_nested_data from unserialize
--FILE--
<?php
$s = 'O:8:"stdClass":00000000';
var_dump(unserialize($s));
?>
--EXPECTF--
Warning: unserialize(): Error at offset 23 of 23 bytes in %s on line %d
bool(false)
