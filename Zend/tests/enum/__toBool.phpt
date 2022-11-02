--TEST--
Enum __toBool
--FILE--
<?php

enum Foo {
    case Bar;

    public function __toBool(): bool {
        return true;
    }
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include magic method __toBool in %s on line %d
