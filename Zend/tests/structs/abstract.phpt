--TEST--
Structs must not be marked as abstract
--FILE--
<?php

abstract struct Box {}

?>
--EXPECTF--
Fatal error: Cannot use the abstract modifier on a struct, as structs are implicitly final in %s on line %d
