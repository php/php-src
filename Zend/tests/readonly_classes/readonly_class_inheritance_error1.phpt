--TEST--
Readonly class cannot extend a non-readonly class
--FILE--
<?php

class Foo
{
}

readonly class Bar extends Foo
{
}

?>
--EXPECTF--
Fatal error: Readonly class Bar cannot extend non-readonly class Foo in %s on line %d
