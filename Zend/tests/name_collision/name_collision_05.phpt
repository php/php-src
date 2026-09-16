--TEST--
Object naming collision error: interface/trait
--FILE--
<?php

interface A { }
trait A { }

?>
--EXPECTF--
Fatal error: Cannot redeclare interface A (previously declared in %s:%d) in %s on line %d
