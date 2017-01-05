--TEST--
Object naming collision error: interface/interface
--FILE--
<?php

interface A { }
interface A { }

?>
--EXPECTF--
Fatal error: Cannot declare interface A, because the name is already in use in %s on line %d
