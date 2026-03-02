--TEST--
Bug #72433: Use After Free Vulnerability in PHP's GC algorithm and unserialize
--FILE--
<?php
$serialized_payload = 'a:3:{i:0;r:1;i:1;r:1;i:2;C:11:"ArrayObject":19:{x:i:0;r:1;;m:a:0:{}}}';
var_dump(unserialize($serialized_payload));
?>
--EXPECTF--
Warning: unserialize(): Error at offset 13 of 69 bytes in %s on line %d
bool(false)
