--TEST--
Operator method rejects string operand parameter type
--FILE--
<?php

readonly class Bad implements Operable {
    public function __mul(string $s): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__mul() has an unsupported operand parameter type in %s on line %d
