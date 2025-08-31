--TEST--
Bug #72581 (previous property undefined in Exception after deserialization)
--FILE--
<?php

$e = new Exception('aaa', 200);

$a = serialize($e);

$b = unserialize($a);

var_dump($b->__toString());
?>
--EXPECTF--
string(%s) "Exception: aaa in %sbug72581.php:%d
Stack trace:
#0 {main}"
