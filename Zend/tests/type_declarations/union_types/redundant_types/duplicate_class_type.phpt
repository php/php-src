--TEST--
Duplicate class type
--FILE--
<?php

function test(): Foo|int|Foo {
}

?>
--EXPECTF--
Fatal error: Duplicate type Foo is redundant in %s on line %d
