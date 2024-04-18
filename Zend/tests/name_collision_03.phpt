--TEST--
Object naming collision error: class/trait
--FILE--
<?php

class A { }
trait A { }

?>
--EXPECTF--
Fatal error: Cannot declare trait A, because the name is already in use (previously declared in %s:%d) in %s on line %d
