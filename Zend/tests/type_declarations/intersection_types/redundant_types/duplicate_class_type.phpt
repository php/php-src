--TEST--
Duplicate class type
--FILE--
<?php

function test(): Foo&A&FOO {
}

?>
--EXPECTF--
Fatal error: Duplicate type FOO is redundant in %s on line %d
