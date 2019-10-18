--TEST--
Duplicate class type
--FILE--
<?php

function test(): Foo|int|FOO {
}

?>
--EXPECTF--
Fatal error: Type FOO is redundant in %s on line %d
