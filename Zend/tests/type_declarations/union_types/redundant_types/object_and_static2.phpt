--TEST--
object and static are redundant 2
--FILE--
<?php

class Test {
    public function foo(): object|static {}
}

?>
--EXPECTF--
Fatal error: Type static|object contains both object and a class type, which is redundant in %s on line %d
