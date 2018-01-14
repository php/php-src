--TEST--
Bug #74101: Unserialize Heap Use-After-Free (READ: 1) in zval_get_type
--FILE--
<?php
$s = 'O:9:"Exception":799999999999999999999999999997:0i:0;a:0:{}i:2;i:0;i:0;R:2;';
var_dump(unserialize($s));
?>
--EXPECTF--
Warning: unserialize(): %s in %sbug74101.php on line %d

Notice: unserialize(): Error at offset 48 of 74 bytes in %sbug74101.php on line %d
bool(false)
