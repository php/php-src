--TEST--
Readonly property cannot be reset after cloning when there is no custom clone handler
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly int $bar,
        public readonly int $baz
    ) {}

    public function wrongCloneOld()
    {
        $instance = clone $this;
        $this->bar++;
    }

    public function wrongCloneNew()
    {
        $instance = clone $this;
        $instance->baz++;
    }
}

$foo = new Foo(1, 1);

try {
    $foo->wrongCloneOld();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->wrongCloneNew();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot modify readonly property Foo::$bar
Cannot modify readonly property Foo::$baz
