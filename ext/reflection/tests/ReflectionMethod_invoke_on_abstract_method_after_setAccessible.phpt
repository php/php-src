--TEST--
ReflectionMethod::invoke() on an abstract method should fail
--FILE--
<?php

abstract class Test {
    abstract static function foo();
}

$rm = new ReflectionMethod('Test', 'foo');
try {
    var_dump($rm->invoke(null));
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Trying to invoke abstract method Test::foo()
