--TEST--
Nested compound patterns require parentheses
--FILE--
<?php
var_dump(42 is Foo|Bar&Baz);
?>
--EXPECTF--
Fatal error: Nested compound pattern must be parenthesized in %s on line %d
