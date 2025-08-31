--TEST--
__sleep can only declare return as array
--FILE--
<?php
class Foo {
    public function __sleep(): bool|int {}
}
?>
--EXPECTF--
Fatal error: Foo::__sleep(): Return type must be array when declared in %s on line %d
