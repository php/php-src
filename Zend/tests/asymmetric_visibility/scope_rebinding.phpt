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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    ($c->bindTo(null, Bar::class))();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($foo->bar);

?>
--EXPECT--
int(2)
Error: Cannot modify private(set) property Foo::$bar from global scope
Error: Cannot modify private(set) property Foo::$bar from scope Bar
int(2)
