--TEST--
Test that the clone property initializer respects visibility
--FILE--
<?php

class Foo
{
    private $bar;
}

$foo = new Foo();

try {
    $foo = clone $foo with {bar: 1};
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot access private property Foo::$bar
