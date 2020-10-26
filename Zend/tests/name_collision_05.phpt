--TEST--
Object naming collision error: interface/trait
--FILE--
<?php

interface A { }
trait A { }

?>
--EXPECTF--
Fatal error: trait A has already been declared in %s on line %d
