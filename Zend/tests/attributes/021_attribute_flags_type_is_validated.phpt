--TEST--
Attribute flags type is validated.
--FILE--
<?php

#[Attribute("foo")]
class A1 { }

?>
--EXPECTF--
Fatal error: Attribute::__construct(): Argument #1 ($flags) must be of type int, string given in %s
