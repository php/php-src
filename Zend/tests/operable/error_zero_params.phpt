--TEST--
Operator method must take at least 1 parameter
--FILE--
<?php

readonly class Bad implements Operable {
    public function __add(): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() must take 1 or 2 parameters in %s on line %d
