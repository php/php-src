--TEST--
Bug #53144 (Segfault in SplObjectStorage::removeAll)
--FILE--
<?php

$o1 = new stdClass;
$o2 = new stdClass;

$b = new SplObjectStorage();
$b[$o1] = "bar";
$b[$o2] = "baz";

var_dump(count($b));
$b->removeAll($b);
var_dump(count($b));

?>
--EXPECT--
int(2)
int(0)
