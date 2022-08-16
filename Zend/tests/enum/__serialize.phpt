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
Fatal error: Enum may not include magic method __serialize in %s on line %d
