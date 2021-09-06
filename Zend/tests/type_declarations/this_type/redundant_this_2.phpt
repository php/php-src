--TEST--
Redundant static|$this type
--FILE--
<?php

class Test {
    public function method(): static|$this {}
}

?>
--EXPECTF--
Fatal error: Type $this|static contains both static and $this, which is redundant in %s on line %d
