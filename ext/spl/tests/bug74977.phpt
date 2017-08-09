--TEST--
Bug #74977:	Recursion leads to crash
--FILE--
<?php

$iterator = new AppendIterator(array("A","A","A"));
$iterator->append($iterator);
var_dump($iterator);
?>
--EXPECTF--
object(AppendIterator)#1 (0) {
}

