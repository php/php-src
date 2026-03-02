--TEST--
static type cannot take part in an intersection type
--FILE--
<?php

class A {
    public function foo(): static&Iterator {}
}

?>
--EXPECTF--
Fatal error: Type static cannot be part of an intersection type in %s on line %d
