--TEST--
Object naming collision error: class/interface
--FILE--
<?php

class A { }
interface A { }

?>
--EXPECTF--
Fatal error: Cannot declare interface A, because the name is already in use in %s on line %d
