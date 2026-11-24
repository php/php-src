--TEST--
Non-Operable parent cannot smuggle invalid operator method into Operable child
--FILE--
<?php

readonly class Base {
    public function &__add(int $x): self {
        return $this;
    }
}

final readonly class Child extends Base implements Operable {
}

?>
--EXPECTF--
Fatal error: Operator method Child::__add() cannot return by reference in %s on line %d
