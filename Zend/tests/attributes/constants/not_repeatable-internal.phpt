--TEST--
Validation of attribute repetition (not allowed; internal attribute)
--FILE--
<?php

#[Deprecated]
#[Deprecated]
const MY_CONST = true;

?>
--EXPECTF--
Fatal error: Attribute "Deprecated" must not be repeated in %s on line %d
