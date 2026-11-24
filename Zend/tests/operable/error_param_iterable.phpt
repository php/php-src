--TEST--
Operator method rejects iterable operand parameter type
--FILE--
<?php

readonly class Bad implements Operable {
    public function __add(iterable $x): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() has an unsupported operand parameter type in %s on line %d
