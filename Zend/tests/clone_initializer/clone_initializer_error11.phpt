--TEST--
Test that the destructor of the cloned object is run
--FILE--
<?php

class Foo
{
    public function __construct(
        public readonly int $bar
    ) {}

    public function __destruct()
    {
        echo "Destruct $this->bar\n";
    }

    public function with()
    {
        return clone $this with {
            "bar" => 1,
            "bar" => 2,
        };
    }
}

$foo = new Foo(0);

try {
    $foo->with();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
Destruct 1
Cannot modify readonly property Foo::$bar
Destruct 0
