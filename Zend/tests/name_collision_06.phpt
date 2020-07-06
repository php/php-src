--TEST--
Object naming collision error: trait/trait
--FILE--
<?php

trait A { }
trait A { }

?>
--EXPECTF--
Fatal error: trait A cannot be declared, because the name is already in use in %s on line %d
