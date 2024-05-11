--TEST--
Object naming collision error: class/class
--FILE--
<?php

class A { }
class A { }

?>
--EXPECTF--
Fatal error: Cannot redeclare class A (previously declared in %s:%d) in %s on line %d
