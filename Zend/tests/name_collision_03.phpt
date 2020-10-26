--TEST--
Object naming collision error: class/trait
--FILE--
<?php

class A { }
trait A { }

?>
--EXPECTF--
Fatal error: trait A has already been declared in %s on line %d
