--TEST--
Redundant object|$this type
--FILE--
<?php

class Test {
    public function method(): object|$this {}
}

?>
--EXPECTF--
Fatal error: Type $this|object contains both object and a class type, which is redundant in %s on line %d
