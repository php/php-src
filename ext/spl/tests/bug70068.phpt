--TEST--
Bug #70068 (Dangling pointer in the unserialization of ArrayObject items)
--FILE--
<?php
$a = unserialize('a:3:{i:0;C:11:"ArrayObject":20:{x:i:0;r:3;;m:a:0:{};}i:1;d:11;i:2;S:31:"AAAAAAAABBBBCCCC\01\00\00\00\04\00\00\00\00\00\00\00\00\00\00";}');
?>
OK
--EXPECT--
OK