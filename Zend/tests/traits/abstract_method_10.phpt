--TEST--
Abstract method in trait using "self" (invalid)
--FILE--
<?php

trait T {
    abstract private function method(self $x): self;
}

class C {
    use T;

    private function method(int $x): int { }
}

?>
===DONE===
--EXPECTF--
Fatal error: Declaration of C::method(int $x): int must be compatible with T::method(C $x): C in %s on line %d
