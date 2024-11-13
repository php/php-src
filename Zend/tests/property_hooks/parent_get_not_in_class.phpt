--TEST--
Using parent::$prop::get() outside of class context
--FILE--
<?php

var_dump(parent::$prop::get());

?>
--EXPECTF--
Fatal error: Cannot use "parent" when no class scope is active in %s on line %d
