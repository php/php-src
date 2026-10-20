--TEST--
Error when attribute does not target constants (internal attribute)
--FILE--
<?php

#[Attribute]
const EXAMPLE = 'Foo';

?>
--EXPECTF--
Fatal error: Attribute "Attribute" cannot target constant (allowed targets: class) in %s on line %d
