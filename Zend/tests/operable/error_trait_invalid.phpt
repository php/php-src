--TEST--
Invalid operator method imported from trait is rejected on Operable class
--FILE--
<?php

trait BadOpTrait {
    public function __add(string $s): self {
        return $this;
    }
}

readonly class Bad implements Operable {
    use BadOpTrait;
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() has an unsupported operand parameter type in %s on line %d
