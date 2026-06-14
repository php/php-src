--TEST--
GH-15833 (Segmentation fault (access null pointer) in ext/spl/spl_array.c)
--CREDITS--
YuanchengJiang
--FILE--
<?php
class C {
    public int $a = 1;
}
$reflector = new ReflectionClass(C::class);
$obj = $reflector->newLazyProxy(function ($obj) {
    $obj = new C();
    return $obj;
});
$recursiveArrayIterator = new RecursiveArrayIterator($obj);
var_dump($recursiveArrayIterator->current());
$recursiveArrayIterator->next();
var_dump($recursiveArrayIterator->current());
?>
--EXPECTF--
Deprecated: ArrayIterator::__construct(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
int(1)
NULL
