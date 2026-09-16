--TEST--
Object naming collision error: class/trait
--FILE--
<?php

class A { }
trait A { }

?>
--EXPECTF--
Fatal error: Cannot redeclare class A (previously declared in %s:%d) in %s on line %d
