--TEST--
Operator method rejects object operand parameter type
--FILE--
<?php

readonly class Bad implements Operable {
    public function __add(object $x): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() has an unsupported operand parameter type in %s on line %d
