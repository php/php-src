--TEST--
Operator method cannot return never
--FILE--
<?php

readonly class Bad implements Operable {
    public function __add(int $x): never {
        throw new Exception();
    }
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() cannot return void, never, or mixed in %s on line %d
