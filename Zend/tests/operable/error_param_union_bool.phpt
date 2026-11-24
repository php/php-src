--TEST--
Operator method rejects disallowed primitive inside union type
--FILE--
<?php

readonly class Bad implements Operable {
    public function __add(int|bool $x): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() has an unsupported operand parameter type in %s on line %d
