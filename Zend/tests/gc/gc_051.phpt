--TEST--
GC 051: Acyclic objects are not added to GC buffer
--FILE--
<?php

function test($x) {
    assert($x); // Prevent inlining
}

enum E {
    case A;
    case B;
    case C;
}

#[AllowDynamicProperties]
class AcyclicC {
    public int $a;
    public string $b;
}

class CyclicC {
    public array $a;
}

echo "Enums\n";
test(E::A);
test(E::B);
test(E::C);
var_dump(gc_status()['roots']);

echo "Acyclic object\n";
$o = new AcyclicC;
test($o);
var_dump(gc_status()['roots']);
unset($o);

echo "Cyclic object\n";
$o = new CyclicC;
test($o);
var_dump(gc_status()['roots']);
unset($o);

echo "Acyclic object with dynamic properties\n";
$o = new AcyclicC;
$o->dyn = 42;
test($o);
var_dump(gc_status()['roots']);
unset($o);

echo "Stateless closure\n";
$o = static function () {};
test($o);
var_dump(gc_status()['roots']);
unset($o);

echo "Closure with bindings\n";
$x = [];
$o = static function () use ($x) {};
unset($x);
test($o);
var_dump(gc_status()['roots']);
unset($o);

echo "Closure with static vars\n";
$o = static function () {
    static $x;
};
test($o);
var_dump(gc_status()['roots']);
unset($o);

echo "Non-static closure\n";
$o = function () {};
test($o);
var_dump(gc_status()['roots']);
unset($o);

echo "Generator\n";
$c = static function () { yield; }; // Not collectable
test($c);
$o = $c(); // Collectable
test($o);
var_dump(gc_status()['roots']);
unset($o);
unset($c);

?>
--EXPECT--
Enums
int(0)
Acyclic object
int(0)
Cyclic object
int(1)
Acyclic object with dynamic properties
int(1)
Stateless closure
int(0)
Closure with bindings
int(1)
Closure with static vars
int(1)
Non-static closure
int(1)
Generator
int(1)
