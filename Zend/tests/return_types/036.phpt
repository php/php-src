--TEST--
__toString can only declare string return type
--FILE--
<?php
class Foo {
    public function __toString(): bool {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of Foo::__toString(): bool must be compatible with Stringable::__toString(): string in %s on line %d
