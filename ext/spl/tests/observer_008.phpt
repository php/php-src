--TEST--
SPL: SplObjectStorage addAll/removeAll
--FILE--
<?php
class A extends SplObjectStorage { }

$o1 = new StdClass;
$o2 = new StdClass;
$o3 = new StdClass;

$a = new A;
$a->attach($o1);
$a->attach($o2);

$b = new SplObjectSTorage();
$b->attach($o2);
$b->attach($o3);

$a->addAll($b);

var_dump($a->count());

$a->detach($o3);
var_dump($a->count());

$a->removeAll($b);
var_dump($a->count());
?>
===DONE===
<?php exit(0); ?>
--EXPECT--
int(3)
int(2)
int(1)
===DONE===
