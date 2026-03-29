--TEST--
Invokable is automatically implemented
--FILE--
<?php

/* Basic auto-implementation */
class Test {
    public function __invoke(): string {
        return "foo";
    }
}

var_dump(new Test instanceof Invokable);
var_dump((new ReflectionClass(Test::class))->getInterfaceNames());

/* Inheritance: child inherits Invokable from parent */
class Child extends Test {}

var_dump(new Child instanceof Invokable);
var_dump((new ReflectionClass(Child::class))->getInterfaceNames());

/* Child overrides __invoke */
class ChildOverride extends Test {
    public function __invoke(): string {
        return "bar";
    }
}

var_dump(new ChildOverride instanceof Invokable);

/* Arbitrary signature: different params and return type */
class Adder {
    public function __invoke(int $a, int $b): int {
        return $a + $b;
    }
}

var_dump(new Adder instanceof Invokable);

/* No params, no return type */
class NoSig {
    public function __invoke() {}
}

var_dump(new NoSig instanceof Invokable);

/* Explicit + implicit: class has __invoke and writes implements Invokable */
class ExplicitAndImplicit implements Invokable {
    public function __invoke(): void {}
}

var_dump(new ExplicitAndImplicit instanceof Invokable);
/* Should appear only once in the interface list */
var_dump((new ReflectionClass(ExplicitAndImplicit::class))->getInterfaceNames());

?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  string(9) "Invokable"
}
bool(true)
array(1) {
  [0]=>
  string(9) "Invokable"
}
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  [0]=>
  string(9) "Invokable"
}
