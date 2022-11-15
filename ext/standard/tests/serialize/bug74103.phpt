--TEST--
Bug #74103: heap-use-after-free when unserializing invalid array size
--FILE--
<?php
var_dump(unserialize('a:7:{i:0;i:04;s:1:"a";i:2;i:9617006;i:4;s:1:"a";i:4;s:1:"a";R:5;s:1:"7";R:3;s:1:"a";R:5;;s:18;}}'));
?>
--EXPECTF--
Warning: unserialize(): Error at offset 64 of 96 bytes in %s on line %d
bool(false)
