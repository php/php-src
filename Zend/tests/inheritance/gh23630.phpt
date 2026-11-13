--TEST--
GH-23630: Interface inheritance depends on the order of the parent interfaces
--FILE--
<?php

interface A {
    public function __construct(?string $name = null);
}

interface B {
    public function __construct(?string $name = null, ?int $id = null);
}

interface C extends A, B {
}

interface D extends B, A {
}

abstract class E implements A, B {
}

abstract class F implements B, A {
}

foreach (['C', 'D', 'E', 'F'] as $inherited) {
    $method = new ReflectionMethod($inherited, '__construct');
    var_dump($method->getDeclaringClass()->getName(), $method->getNumberOfParameters());
}

?>
--EXPECT--
string(1) "B"
int(2)
string(1) "B"
int(2)
string(1) "B"
int(2)
string(1) "B"
int(2)
