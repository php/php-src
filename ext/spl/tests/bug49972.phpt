--TEST--
Bug #49972 (AppendIterator undefined function crash)
--FILE--
<?php

$iterator = new AppendIterator();
$iterator->undefined();

?>
--EXPECTF--
Fatal error: Call to undefined method AppendIterator::undefined() in %s on line %d
