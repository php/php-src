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
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $foo->wrongCloneNew();
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot modify readonly property Foo::$bar
Error: Cannot modify readonly property Foo::$baz
