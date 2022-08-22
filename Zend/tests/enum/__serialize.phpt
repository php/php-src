--TEST--
Enum __serialize
--FILE--
<?php

enum Foo {
    case Bar;

    public function __serialize(): array {
        return $this->cases();
    }
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include magic method __serialize in %s on line %d
