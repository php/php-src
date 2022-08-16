--TEST--
Enum __debugInfo
--FILE--
<?php

enum Foo {
    case Bar;

    public function __debugInfo(): array {
        return $this->cases();
    }
}

?>
--EXPECTF--
Fatal error: Enum may not include magic method __debugInfo in %s on line %d
