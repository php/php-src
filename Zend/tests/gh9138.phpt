--TEST--
GH-9138: NULL pointer dereference when fetching property of "bad" list in constant expression
--FILE--
<?php

#[Attribute([,]->e)]
class Foo {}

?>
--EXPECTF--
Fatal error: Cannot use empty array elements in arrays in %s on line %d
