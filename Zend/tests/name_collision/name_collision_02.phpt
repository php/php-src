--TEST--
Object naming collision error: class/interface
--FILE--
<?php

class A { }
interface A { }

?>
--EXPECTF--
Fatal error: Cannot redeclare class A (previously declared in %s:%d) in %s on line %d
