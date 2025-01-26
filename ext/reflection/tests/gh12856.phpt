--TEST--
GH-12856 (ReflectionClass::getStaticPropertyValue() returns UNDEF zval for uninitialized typed properties)
--FILE--
<?php

class Bug {
    private static $untyped;
    private static int $typed1;
    private static int $typed2 = 3;
}

$rc = new ReflectionClass(Bug::class);
var_dump($rc->getStaticPropertyValue('untyped'));
try {
    var_dump($rc->getStaticPropertyValue('typed1'));
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($rc->getStaticPropertyValue('typed1', 1));
var_dump($rc->getStaticPropertyValue('typed2'));

?>
--EXPECT--
NULL
Property Bug::$typed1 is not initialized and no default was provided
int(1)
int(3)
