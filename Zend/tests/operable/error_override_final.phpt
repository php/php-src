--TEST--
Concrete operator method on non-final Operable class must be declared final
--FILE--
<?php

readonly class ParentOp implements Operable {
    public function __add(int $x): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method ParentOp::__add() on non-final class must be declared final in %s on line %d
