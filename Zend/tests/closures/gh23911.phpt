--TEST--
GH-23911: Cached static closure keeps static:: of the first called class
--FILE--
<?php

class A {
    public static function name(): string {
        return (static fn () => static::class)();
    }

    public static function create(): static {
        return (static fn () => new static())();
    }

    public function calledClass(): string {
        return (static function () {
            return get_called_class();
        })();
    }
}
class B extends A {}
class C extends A {}

var_dump(B::name(), C::name(), B::name(), A::name());
var_dump(B::create()::class, C::create()::class);
var_dump((new B)->calledClass(), (new C)->calledClass());

function f() {
    return static fn () => 42;
}
var_dump(f() === f());
var_dump(B::name(...) !== C::name(...));

?>
--EXPECT--
string(1) "B"
string(1) "C"
string(1) "B"
string(1) "A"
string(1) "B"
string(1) "C"
string(1) "B"
string(1) "C"
bool(true)
bool(true)
