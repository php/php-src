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
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test);

?>
--EXPECT--
Cannot assign array to reference held by property Test::$prop of type int
object(Test)#1 (1) {
  ["prop"]=>
  int(42)
}
