--TEST--
Operator method cannot be variadic
--FILE--
<?php

readonly class Bad implements Operable {
    public function __add(int ...$xs): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() cannot be variadic in %s on line %d
