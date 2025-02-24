--TEST--
Object naming collision error: trait/trait
--FILE--
<?php

trait A { }
trait A { }

?>
--EXPECTF--
Fatal error: Cannot redeclare trait A (previously declared in %s:%d) in %s on line %d
