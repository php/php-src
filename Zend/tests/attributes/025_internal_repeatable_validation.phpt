--TEST--
Internal attribute targets are validated.
--FILE--
<?php

#[Attribute]
#[Attribute]
class A1 { }

?>
--EXPECTF--
Fatal error: Attribute "Attribute" must not be repeated in %s
