--TEST--
Bug #70852 Segfault getting NULL offset of an ArrayObject
--FILE--
<?php
$y = new ArrayObject();
echo $y[NULL];
?>
===DONE===
--EXPECTF--
Notice: Undefined index:  in %s on line %d
===DONE===
