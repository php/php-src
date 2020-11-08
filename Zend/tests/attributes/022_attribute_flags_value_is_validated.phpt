--TEST--
Attribute flags value is validated.
--FILE--
<?php

#[Attribute(-1)]
class A1 { }

?>
--EXPECTF--
Fatal error: Invalid attribute flags specified in %s
