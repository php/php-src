--TEST--
Operator method cannot be protected
--FILE--
<?php

readonly class Bad implements Operable {
    protected function __add(int $x): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() must be public and non-static in %s on line %d
