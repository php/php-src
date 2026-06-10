--TEST--
Serialization of const-expr closures with inheritance and traits
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class A {
    public function __construct(public mixed $cb = null) {}
}

class Base {
    #[A(static function () { return 'base'; })]
    public int $p = 0;
}

class Child extends Base {}

trait T {
    public function m(
        #[A(static function () { return 'trait'; })]
        $x = null,
    ) {}
}

class UsesTrait {
    use T;
}

// Attribute on an inherited property: the closure is scoped to the
// declaring class and the reference uses that class.
$c = (new ReflectionProperty(Child::class, 'p'))->getAttributes()[0]->getArguments()[0];
var_dump((new ReflectionFunction($c))->getClosureScopeClass()->name);
$payload = serialize($c);
var_dump(str_contains($payload, '"Base"'));
var_dump(unserialize($payload)());

// Attribute on a parameter of a trait method: the copied method is scoped
// to the using class and the reference resolves through it.
$c = (new ReflectionParameter([UsesTrait::class, 'm'], 'x'))->getAttributes()[0]->getArguments()[0];
var_dump((new ReflectionFunction($c))->getClosureScopeClass()->name);
$u = unserialize(serialize($c));
var_dump($u());

?>
--EXPECT--
string(4) "Base"
bool(true)
string(4) "base"
string(9) "UsesTrait"
string(5) "trait"
