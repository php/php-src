--TEST--
self type cannot take part in an intersection type
--FILE--
<?php

class A {
    public function foo(): self&Iterator {}
}

?>
--EXPECTF--
Fatal error: Type self cannot be part of an intersection type in %s on line %d
