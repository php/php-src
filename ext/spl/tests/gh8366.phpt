--TEST--
Bug GH-8366 (ArrayIterator may leak when calling __construct())
--FILE--
<?php
$it = new \ArrayIterator();
foreach ($it as $elt) {}
$it->__construct([]);
?>
--EXPECT--
