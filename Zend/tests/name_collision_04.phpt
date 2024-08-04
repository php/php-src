--TEST--
Object naming collision error: interface/interface
--FILE--
<?php

interface A { }
interface A { }

?>
--EXPECTF--
Fatal error: Cannot redeclare interface A (previously declared in %s:%d) in %s on line %d
