--TEST--
The readonly and final class modifiers can be defined in the same time
--FILE--
<?php

final readonly class Foo
{
}

readonly class Bar extends Foo
{
}

?>
--EXPECTF--
Fatal error: Class Bar cannot extend final class Foo in %s on line %d
