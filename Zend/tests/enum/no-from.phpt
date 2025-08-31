--TEST--
Enum no manual from method
--FILE--
<?php

enum Foo: int {
    case Bar = 0;

    public static function from(string|int $value): self {
        return $this;
    }
}

?>
--EXPECTF--
Fatal error: Cannot redeclare Foo::from() in %s on line %d
