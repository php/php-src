--TEST--
object and static are redundant
--FILE--
<?php

class Test {
    public function foo(): static|object {}
}

?>
--EXPECTF--
Fatal error: Type static|object contains both object and a class type, which is redundant in %s on line %d
