--TEST--
Lazy objects: feedback 007
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

?>
==DONE==
--EXPECT--
==DONE==
