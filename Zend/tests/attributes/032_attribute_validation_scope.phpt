--TEST--
Validation for "Attribute" does not use a scope when evaluating constant ASTs
--FILE--
<?php
#[Attribute(parent::x)]
class x extends y {}
?>
--EXPECTF--
Fatal error: Cannot access "parent" when no class scope is active in %s on line %d
