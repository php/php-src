--TEST--
Object naming collision error: class/class
--FILE--
<?php

class A { }
class A { }

?>
--EXPECTF--
Fatal error: class A has already been declared in %s on line %d
