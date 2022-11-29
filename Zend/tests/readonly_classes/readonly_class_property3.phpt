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
Fatal error: Static property Foo::$bar cannot be readonly in %s on line %d
