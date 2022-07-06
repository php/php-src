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
Fatal error: Foo::__toString(): Return type must be string when declared in %s on line %d
