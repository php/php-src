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
    static $counter = 0;
    throw new Error('nope ' . ($counter++));
});
$recursiveArrayIterator = new RecursiveArrayIterator($obj);
try {
    var_dump($recursiveArrayIterator->current());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($recursiveArrayIterator->current());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $recursiveArrayIterator->next();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($recursiveArrayIterator->current());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
nope 0
nope 1
nope 2
nope 3
