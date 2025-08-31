--TEST--
Changing scope with Closure::bindTo() does not confuse asymmetric visibility
--FILE--
<?php

class Foo {
    public private(set) int $bar = 1;
}
class Bar {}

$foo = new Foo();

$c = function () use ($foo) {
    $foo->bar++;
};

($c->bindTo(null, Foo::class))();
var_dump($foo->bar);
try {
    $c();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    ($c->bindTo(null, Bar::class))();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($foo->bar);

?>
--EXPECT--
int(2)
Cannot modify private(set) property Foo::$bar from global scope
Cannot modify private(set) property Foo::$bar from scope Bar
int(2)
