--TEST--
Backed enums type can't be union
--FILE--
<?php

enum Foo: int|string {}

?>
--EXPECTF--
Fatal error: Enum backing type must be int or string, string|int given in %s on line %d
