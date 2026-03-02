--TEST--
Duplicate class type
--FILE--
<?php

function test(): Foo|int|FOO {
}

?>
--EXPECTF--
Fatal error: Duplicate type FOO is redundant in %s on line %d
