--TEST--
Test that readonly properties cannot be assigned after "clone with"
--FILE--
<?php

class Foo
{
    public function __construct(
        public readonly int $bar
    ) {}

    public function with()
    {
        return clone $this with {
            bar: 1,
        };
    }

    public function assign()
    {
        $this->bar = 2;
    }
}

$foo1 = new Foo(0);

$foo2 = $foo1->with();

try {
    $foo1->assign();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo2->assign();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

$foo3 = $foo2->with(); // The same as above, but now using cache slots

try {
    $foo1->assign(); // The same as above, but now using cache slots
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo2->assign(); // The same as above, but now using cache slots
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo3->assign(); // The same as above, but now using cache slots
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot modify readonly property Foo::$bar
Cannot modify readonly property Foo::$bar
Cannot modify readonly property Foo::$bar
Cannot modify readonly property Foo::$bar
Cannot modify readonly property Foo::$bar
