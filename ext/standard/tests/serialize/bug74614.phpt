--TEST--
Bug #74614: Use-after-free in PHP7's unserialize()
--FILE--
<?php

unserialize('a:3020000000000000000000000000000001:{i:0;a:0:{}i:1;i:2;i:2;i:3;i:3;i:4;i:4;i:5;i:5;i:6;i:6;i:7;i:7;i:8;i:8;R:2;}');

?>
--EXPECTF--
Notice: unserialize(): Error at offset %d of 113 bytes in %s on line %d
