--TEST--
Object naming collision error: class/interface
--FILE--
<?php

class A { }
interface A { }

?>
--EXPECTF--
Fatal error: interface A has already been declared in %s on line %d
