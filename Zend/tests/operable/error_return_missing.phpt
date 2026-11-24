--TEST--
Operator method must declare a return type
--FILE--
<?php

readonly class Bad implements Operable {
    public function __add(int $x) {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() must declare a return type in %s on line %d
