--TEST--
Concrete parameterized types used in type expressions
--FILE--
<?php

abstract class AbstractTest<T> {
    public function method(T $param) {
        var_dump($param);
    }
}

abstract class AbstractPassthru<T> extends AbstractTest<T> {
}

abstract class AbstractDefaulted<T=int> extends AbstractTest<T> {
}

class ConcreteInt extends AbstractTest<int> {
}

class ConcreteString extends AbstractTest<string> {
}

class ConcreteIntPassthru extends AbstractPassthru<int> {
}

class ConcreteIntDefaulted extends AbstractDefaulted {
}
class ConcreteStringDefaulted extends AbstractDefaulted<string> {
}

function test(AbstractTest<int> $test) {
    $test->method(42);
}

test(new ConcreteInt);
test(new ConcreteIntPassthru);
test(new ConcreteIntDefaulted);
try {
    test(new ConcreteString);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

function test2(AbstractDefaulted $test) {
    $test->method(42);
}

function test3(AbstractDefaulted<int> $test) {
    $test->method(42);
}

test2(new ConcreteIntDefaulted);
test3(new ConcreteIntDefaulted);
try {
    test2(new ConcreteStringDefaulted);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    test3(new ConcreteStringDefaulted);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
int(42)
int(42)
int(42)
test(): Argument #1 ($test) must be of type AbstractTest<int>, ConcreteString given, called in %s on line %d
int(42)
int(42)
test2(): Argument #1 ($test) must be of type AbstractDefaulted, ConcreteStringDefaulted given, called in %s on line %d
test3(): Argument #1 ($test) must be of type AbstractDefaulted<int>, ConcreteStringDefaulted given, called in %s on line %d
