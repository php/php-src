--TEST--
SPL: SplObjectStorage addAll/removeAll
--FILE--
<?php
class A extends SplObjectStorage { }

$o1 = new stdClass;
$o2 = new stdClass;
$o3 = new stdClass;

$a = new A;
$a->offsetSet($o1);
$a->offsetSet($o2);

$b = new SplObjectStorage();
$b->offsetSet($o2);
$b->offsetSet($o3);

$a->offsetUnset($b);

var_dump($a->count());

$a->offsetUnset($o3);
var_dump($a->count());

$a->removeAll($b);
var_dump($a->count());
?>
--EXPECT--
int(2)
int(2)
int(1)
