--TEST--
Bug #79839: array_walk() does not respect property types
--FILE--
<?php

class Test {
    public int $prop = 42;
}

$test = new Test;
try {
    array_walk($test, function(&$ref) {
        $ref = []; // Should throw
    });
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test);

?>
--EXPECTF--
Deprecated: array_walk(): Passing an object for argument #1 $array to array_walk() is deprecated, call get_object_vars() first instead in %s on line %d
TypeError: Cannot assign array to reference held by property Test::$prop of type int
object(Test)#1 (1) {
  ["prop"]=>
  int(42)
}
