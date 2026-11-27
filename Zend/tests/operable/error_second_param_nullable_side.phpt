--TEST--
Second parameter of operator method cannot be nullable OperandSide
--FILE--
<?php

readonly class Bad implements Operable {
    public function __sub(self $o, ?OperandSide $side): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: The second parameter of operator method Bad::__sub() must be OperandSide in %s on line %d
