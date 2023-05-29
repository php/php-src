--TEST--
Test that readonly properties cannot be assigned in "clone with" multiple times
--FILE--
<?php

class Foo
{
    public function __construct(
        public readonly int $bar
    ) {}

    public function with()
    {
        return clone $this with [
            "bar" => 1,
            "bar" => 2,
        ];
    }
}

$foo = new Foo(0);

try {
    $foo->with();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->with();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot modify readonly property Foo::$bar
Cannot modify readonly property Foo::$bar
