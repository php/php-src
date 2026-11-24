--TEST--
Operator method first parameter cannot be passed by reference
--FILE--
<?php

final readonly class Bad implements Operable {
    public function __add(int &$x): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() parameters cannot be passed by reference in %s on line %d
