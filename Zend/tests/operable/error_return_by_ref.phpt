--TEST--
Operator methods cannot return by reference
--FILE--
<?php

final readonly class Bad implements Operable {
    public function &__add(int $x): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() cannot return by reference in %s on line %d
