--TEST--
Promoted properties of a readonly class are implicitly declared as readonly
--FILE--
<?php

readonly class Foo
{
    public function __construct(
        public int $bar
    ) {}
}

$foo = new Foo(1);

try {
    $foo->bar = 2;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot modify readonly property Foo::$bar
