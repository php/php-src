--TEST--
parent type cannot take part in an intersection type when unresolved
--FILE--
<?php

trait T {
    public function foo(): parent&Iterator {}
}

?>
--EXPECTF--
Fatal error: Type parent cannot be part of an intersection type in %s on line %d
