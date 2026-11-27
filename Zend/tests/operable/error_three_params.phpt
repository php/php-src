--TEST--
Operator method cannot take more than 2 parameters
--FILE--
<?php

readonly class Bad implements Operable {
    public function __div(int $x, OperandSide $side, int $extra): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__div() must take 1 or 2 parameters in %s on line %d
