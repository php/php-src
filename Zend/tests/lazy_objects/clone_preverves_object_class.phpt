--TEST--
Lazy objects: clone returns an object of the same class
--FILE--
<?php

class A {
    public function __construct(
        public string $property,
    ) {}
}
class B extends A {
    public function foo() { }
}

function only_b(B $b) { $b->foo(); }

$r = new ReflectionClass(B::class);
$b = $r->newLazyProxy(function ($obj) {
    return new A('value');
});

$b->property = 'init_please';

$clone = clone $b;
only_b($b);
only_b($clone);

var_dump($b::class);
var_dump($clone::class);

?>
==DONE==
--EXPECT--
string(1) "B"
string(1) "B"
==DONE==
