--TEST--
Operator method parameters cannot have default values
--FILE--
<?php

readonly class Bad implements Operable {
    public function __mod(int $x, OperandSide $side = OperandSide::Left): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__mod() parameters cannot have default values in %s on line %d
