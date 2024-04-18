--TEST--
Object naming collision error: interface/interface
--FILE--
<?php

interface A { }
interface A { }

?>
--EXPECTF--
Fatal error: Cannot declare interface A, because the name is already in use (previously declared in %s:%d) in %s on line %d
