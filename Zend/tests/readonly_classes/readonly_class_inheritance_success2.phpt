--TEST--
Non-readonly class can extend a readonly class
--FILE--
<?php

readonly class Foo
{
}

class Bar extends Foo
{
}

?>
--EXPECT--
