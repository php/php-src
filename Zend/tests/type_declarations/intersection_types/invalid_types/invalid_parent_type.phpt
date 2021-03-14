--TEST--
parent type cannot take part in an intersection type
--FILE--
<?php

class A {}

class B extends A {
    public function foo(): parent&Iterator {}
}

?>
--EXPECTF--
Fatal error: Type parent cannot be part of an intersection type in %s on line %d
