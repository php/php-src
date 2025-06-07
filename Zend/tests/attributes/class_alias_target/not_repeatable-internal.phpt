--TEST--
Validation of attribute repetition (not allowed; internal attribute)
--FILE--
<?php

#[ClassAlias('Other', [new Deprecated(), new Deprecated()])]
class Demo {}

?>
--EXPECTF--
Fatal error: Attribute "Deprecated" must not be repeated in %s on line %d
