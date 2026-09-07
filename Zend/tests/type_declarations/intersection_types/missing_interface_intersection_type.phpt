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
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$c = new Collection();
$a = new A();

try {
    $c->intersect = $a;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bar($a);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
TypeError: foo(): Return value must be of type X&Y, A returned
TypeError: Cannot assign A to property Collection::$intersect of type X&Y
TypeError: bar(): Argument #1 ($o) must be of type X&Y, A given, called in %s on line %d
