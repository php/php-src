--TEST--
Non-readonly class cannot extend a readonly class
--FILE--
<?php

readonly class Foo
{
}

class Bar extends Foo
{
}

?>
--EXPECTF--
Fatal error: Non-readonly class Bar cannot extend readonly class Foo in %s on line %d
