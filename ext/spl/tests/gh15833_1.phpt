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
--EXPECT--
int(1)
NULL
