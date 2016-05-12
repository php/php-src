--TEST--
Using $this when out of context
--FILE--
<?php

$this->a = 1;

?>
--EXPECTF--
Warning: Creating default object from empty value in %s on line %d
