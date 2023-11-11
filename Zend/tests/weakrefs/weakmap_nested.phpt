--TEST--
Nested WeakMap for same object key
--FILE--
<?php
$obj = new stdClass;
$map = new WeakMap;
$map2 = new WeakMap;
$map[$obj] = $map2;
$map2[$obj] = 1;
unset($map2);
unset($obj);
?>
===DONE===
--EXPECT--
===DONE===
