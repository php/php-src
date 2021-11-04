--TEST--
__toString() from trait with invalid return type
--FILE--
<?php

trait T {
    public function __toString(): int {
        return "ok";
    }
}

class C {
    use T;
}

var_dump(new C instanceof Stringable);

?>
--EXPECTF--
Fatal error: Declaration of T::__toString(): int must be compatible with Stringable::__toString(): string in %s on line %d
