--TEST--
Operator method cannot return mixed
--FILE--
<?php

readonly class Bad implements Operable {
    public function __add(int $x): mixed {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() cannot return void, never, or mixed in %s on line %d
