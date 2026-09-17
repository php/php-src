--TEST--
Readonly classes cannot use dynamic properties
--FILE--
<?php

readonly class Foo
{
}

$foo = new Foo();

try {
    $foo->bar = 1;
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot create dynamic property Foo::$bar
