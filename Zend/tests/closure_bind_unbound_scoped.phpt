--TEST--
Closure::bind and ::bindTo unbound_scoped parameter
--FILE--
<?php

$foo = function () {};
$foo = $foo->bindTo(NULL, "StdClass", true);
// We need to check that you can call an unbound scoped closure without binding it (good idea or no)
$foo();

class FooBar {
    private $x = 3;
}
$foo = function () {
    var_dump($this->x);
};

// This should create a closure scoped to FooBar but unbound, not static
$foo = $foo->bindTo(NULL, "FooBar", true);

// As it is unbound, not static, this will work
$foo->call(new FooBar);

$foo = function () {
    var_dump($this->x);
};
// Make sure ::bind() works the same
$foo = Closure::bind($foo, NULL, "FooBar", true);
$foo->call(new FooBar);

// Ensure that binding and having it unscoped will not prevent an E_NOTICE for non-static methods called statically
class Qux {
    function notStatic() {}
}
$x = new ReflectionMethod("Qux::notStatic");
$x = $x->getClosure(new Qux);
$x = $x->bindTo(NULL, "Qux", true);
$x();

// Ensure that binding and having it unscoped will not prevent fatal error for trying to call an internal class's method with $this as NULL
$x = new ReflectionMethod("Closure::bindTo");
$x = $x->getClosure(function () {});
$x = $x->bindTo(NULL, "Closure", true);
$x();
?>
--EXPECTF--
int(3)
int(3)

Strict Standards: Non-static method Qux::notStatic() should not be called statically in %s on line 35

Fatal error: Non-static method Closure::bindTo() cannot be called statically in %s on line 41