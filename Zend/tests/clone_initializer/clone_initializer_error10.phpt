--TEST--
Test that clone with cannot be used on public readonly properties from outside the class.
--FILE--
<?php

class Foo
{
    public readonly int $baz;

    public function __construct(
        public readonly int $bar
    ) {}
}

$foo1 = new Foo(0);

try {
    $foo2 = clone $foo1 with {bar: 1};
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo2 = clone $foo1 with {baz: 1};
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot modify readonly property Foo::$bar from global scope
Cannot initialize readonly property Foo::$baz from global scope
