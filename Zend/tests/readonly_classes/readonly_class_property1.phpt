--TEST--
Normal properties of a readonly class are implicitly declared as readonly
--FILE--
<?php

readonly class Foo
{
    public int $bar;

    public function __construct() {
        $this->bar = 1;
    }
}

$foo = new Foo();

try {
    $foo->bar = 2;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot modify readonly property Foo::$bar
