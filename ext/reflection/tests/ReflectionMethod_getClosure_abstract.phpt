--TEST--
ReflectionMethod::getClosure() on abstract methods
--FILE--
<?php

abstract class A {
    abstract public function instance();
    abstract public static function make();
}

class B extends A implements Countable {
    public function instance() {}
    public static function make() {}
    public function count(): int { return 0; }
}

$methods = [
    [new ReflectionMethod(A::class, 'instance'), new B()],
    [new ReflectionMethod(A::class, 'make'), null],
    [new ReflectionMethod(Countable::class, 'count'), new B()],
    [new ReflectionMethod(UnitEnum::class, 'cases'), null],
];

foreach ($methods as [$method, $object]) {
    try {
        $method->getClosure($object);
    } catch (Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
ReflectionException: Trying to create a closure from abstract method A::instance()
ReflectionException: Trying to create a closure from abstract method A::make()
ReflectionException: Trying to create a closure from abstract method Countable::count()
ReflectionException: Trying to create a closure from abstract method UnitEnum::cases()
