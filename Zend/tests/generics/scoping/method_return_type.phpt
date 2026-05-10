--TEST--
Scoping: T from class scope is visible in method return types
--FILE--
<?php
class Animal {}
class Container<T : Animal> {
    public function get(): T { return new Animal; }
}
$rm = (new ReflectionClass('Container'))->getMethod('get');
echo $rm->getReturnType()->getName(), "\n";
?>
--EXPECT--
Animal
