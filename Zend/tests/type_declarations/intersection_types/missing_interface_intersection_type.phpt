--TEST--
Missing one element of intersection type
--FILE--
<?php

interface X {}
interface Y {}
interface Z {}

class A implements X {}

class Collection {
    public X&Y $intersect;
}

function foo(): X&Y {
    return new A();
}

function bar(X&Y $o): void {
    var_dump($o);
}

try {
    $o = foo();
    var_dump($o);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

$c = new Collection();
$a = new A();

try {
    $c->intersect = $a;
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    bar($a);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
foo(): Return value must be of type X&Y, A returned
Cannot assign A to property Collection::$intersect of type X&Y
bar(): Argument #1 ($o) must be of type X&Y, A given, called in %s on line %d
