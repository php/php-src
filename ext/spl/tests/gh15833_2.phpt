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
    throw new Error('foo');
});
$recursiveArrayIterator = new RecursiveArrayIterator($obj);
try {
    var_dump($recursiveArrayIterator->current());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($recursiveArrayIterator->current());
$recursiveArrayIterator->next();
var_dump($recursiveArrayIterator->current());
?>
--EXPECT--
foo
NULL
NULL
