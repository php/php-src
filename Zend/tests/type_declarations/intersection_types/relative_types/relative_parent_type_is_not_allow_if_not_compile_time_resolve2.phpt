--TEST--
parent type cannot take part in an intersection type if not resolvable at compile time
--FILE--
<?php

trait T {
    public function foo(): PARENT&Iterator {}
}

?>
DONE
--EXPECTF--
Fatal error: Type PARENT cannot be part of an intersection type in %s on line %d
