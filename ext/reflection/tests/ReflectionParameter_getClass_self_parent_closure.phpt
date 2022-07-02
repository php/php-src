--TEST--
Using invalid self/parent types in closure
--FILE--
<?php

$fn1 = function(self $x) {};
try {
    (new ReflectionFunction($fn1))->getParameters()[0]->getClass();
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

$fn2 = function(parent $x) {};
try {
    (new ReflectionFunction($fn2))->getParameters()[0]->getClass();
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

class Test {}
$fn3 = (function(parent $x) {})->bindTo(new Test, Test::class);
try {
    (new ReflectionFunction($fn3))->getParameters()[0]->getClass();
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
Parameter uses "self" as type but function is not a class member

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
Parameter uses "parent" as type but function is not a class member

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
Parameter uses "parent" as type although class does not have a parent
