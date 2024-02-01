--TEST--
Enum __toString
--FILE--
<?php

enum Foo {
    case Bar;

    public function __toString(): string {
        return $this->name;
    }
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include magic method __toString in %s on line %d
