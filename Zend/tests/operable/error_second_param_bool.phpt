--TEST--
Second parameter of operator method must be OperandSide (rejects bool)
--FILE--
<?php

readonly class Bad implements Operable {
    public function __sub(self $o, bool $rev): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: The second parameter of operator method Bad::__sub() must be OperandSide in %s on line %d
