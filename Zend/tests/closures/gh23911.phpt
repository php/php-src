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

trait T {
    public static function names(): array {
        return (static fn () => [self::class, static::class])();
    }
}
class U1 { use T; }
class U2 { use T; }
class U3 extends U1 {}
var_dump(U1::names(), U2::names(), U3::names());

class N {
    public static function nested(): string {
        return (static fn () => (static fn () => static::class)())();
    }
}
class N2 extends N {}
var_dump(N::nested(), N2::nested(), N::nested());

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
array(2) {
  [0]=>
  string(2) "U1"
  [1]=>
  string(2) "U1"
}
array(2) {
  [0]=>
  string(2) "U2"
  [1]=>
  string(2) "U2"
}
array(2) {
  [0]=>
  string(2) "U1"
  [1]=>
  string(2) "U3"
}
string(1) "N"
string(2) "N2"
string(1) "N"
