--TEST--
Operator method cannot be private
--FILE--
<?php

readonly class Bad implements Operable {
    private function __sub(int $x): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__sub() must be public and non-static in %s on line %d
