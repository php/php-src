--TEST--
Object naming collision error: trait/trait
--FILE--
<?php

trait A { }
trait A { }

?>
--EXPECTF--
Fatal error: trait A has already been declared in %s on line %d
