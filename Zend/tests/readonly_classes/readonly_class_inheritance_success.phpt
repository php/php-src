--TEST--
Readonly class can extend a readonly class
--FILE--
<?php

readonly class Foo
{
}

readonly class Bar extends Foo
{
}

?>
--EXPECT--
