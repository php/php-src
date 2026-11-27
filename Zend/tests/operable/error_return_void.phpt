--TEST--
Operator method cannot return void
--FILE--
<?php

readonly class Bad implements Operable {
    public function __add(int $x): void {}
}

?>
--EXPECTF--
Fatal error: Operator method Bad::__add() cannot return void, never, or mixed in %s on line %d
