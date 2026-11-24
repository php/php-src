--TEST--
Operator method cannot be static
--FILE--
<?php

readonly class Bad implements Operable {
    public static function __mul(int $x): self {
        return new self();
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__mul() must be public and non-static in %s on line %d
