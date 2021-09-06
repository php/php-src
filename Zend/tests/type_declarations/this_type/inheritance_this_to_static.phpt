--TEST--
Cannot replace $this with static
--FILE--
<?php

class A {
    public function method(): $this {}
}
class B extends A {
    public function method(): static {}
}

?>
--EXPECTF--
Fatal error: Declaration of B::method(): static must be compatible with A::method(): $this in %s on line %d
