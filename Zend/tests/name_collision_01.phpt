--TEST--
Object naming collision error: class/class
--FILE--
<?php

class A { }
class A { }

?>
--EXPECTF--
Fatal error: class A cannot be declared, because the name is already in use in %s on line %d
