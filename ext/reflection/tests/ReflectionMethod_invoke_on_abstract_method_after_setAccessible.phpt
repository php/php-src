--TEST--
ReflectionMethod::invoke() on an abstract method should fail even after calling setAccessible(true)
--FILE--
<?php

abstract class Test {
    abstract static function foo();
}

$rm = new ReflectionMethod('Test', 'foo');
$rm->setAccessible(true);
try {
    var_dump($rm->invoke(null));
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Trying to invoke abstract method Test::foo()
