--TEST--
Trait-imported concrete operator method on non-final Operable class must be final
--FILE--
<?php

trait AddTrait {
    public function __add(int $x): self {
        return $this;
    }
}

readonly class ParentOp implements Operable {
    use AddTrait;
}

?>
--EXPECTF--
Fatal error: Operator method ParentOp::__add() on non-final class must be declared final in %s on line %d
