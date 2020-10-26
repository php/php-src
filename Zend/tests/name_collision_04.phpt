--TEST--
Object naming collision error: interface/interface
--FILE--
<?php

interface A { }
interface A { }

?>
--EXPECTF--
Fatal error: interface A has already been declared in %s on line %d
