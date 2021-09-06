--TEST--
Cannot replace $this with self
--FILE--
<?php

class A {
    public function method(): $this {}
}
class B extends A {
    public function method(): self {}
}

?>
--EXPECTF--
Fatal error: Declaration of B::method(): B must be compatible with A::method(): $this in %s on line %d
