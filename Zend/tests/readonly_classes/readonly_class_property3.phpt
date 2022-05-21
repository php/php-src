--TEST--
Declaring static property for a readonly class is forbidden
--FILE--
<?php

readonly class Foo
{
    public static int $bar;
}

?>
--EXPECTF--
Fatal error: Readonly class Foo cannot declare static properties in %s on line %d
