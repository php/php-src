--TEST--
GH-24615 (ArrayObject backing storage cycle causes unbounded recursion)
--FILE--
<?php
$a = new ArrayObject([1]);
$b = new ArrayObject($a);
try {
    $a->exchangeArray($b);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count($a), count($b));

/* Longer cycle, through an ArrayIterator */
$c = new ArrayIterator($b);
try {
    $a->exchangeArray($c);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->__construct($c);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count($a), count($b), count($c));

/* Cycle built by __unserialize() */
$d = new ArrayObject([2]);
try {
    $a->__unserialize([0, $d, []]);
    $d->__unserialize([0, $a, []]);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count($a), count($d));

/* Using an object as its own storage and sharing storage are still allowed */
$e = new ArrayObject([3]);
$e->exchangeArray($e);
$f = new ArrayObject($b);
var_dump(count($e), count($f));
?>
--EXPECT--
Cannot use ArrayObject as storage of ArrayObject, as it would create a cycle
int(1)
int(1)
Cannot use ArrayIterator as storage of ArrayObject, as it would create a cycle
Cannot use ArrayIterator as storage of ArrayObject, as it would create a cycle
int(1)
int(1)
int(1)
Cannot use ArrayObject as storage of ArrayObject, as it would create a cycle
int(1)
int(1)
int(0)
int(1)
