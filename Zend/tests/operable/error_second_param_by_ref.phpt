--TEST--
Operator method second parameter (OperandSide) cannot be passed by reference
--FILE--
<?php

final readonly class Bad implements Operable {
    public function __sub(int $x, OperandSide &$side): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__sub() parameters cannot be passed by reference in %s on line %d
