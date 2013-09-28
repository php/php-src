--TEST--
Automatic property initialization: Outside of class error
--FILE--
<?php

function test($this->foo) {}

?>
--EXPECTF--
Fatal error: Automatic property initialization can only be used inside a class in %s on line %d
