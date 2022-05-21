--TEST--
The readonly class modifier can only be added once
--FILE--
<?php

readonly readonly class Foo
{
}

?>
--EXPECTF--
Fatal error: Multiple readonly modifiers are not allowed in %s on line %d
