--TEST--
Enum no manual cases method
--FILE--
<?php

enum Foo {
    case Bar;

    public static function cases(): array {
        return [];
    }
}

?>
--EXPECTF--
Fatal error: Cannot redeclare Foo::cases() in %s on line %d
