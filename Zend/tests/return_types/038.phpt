--TEST--
__serialize can only declare array as return type
--FILE--
<?php
class Foo {
    public function __serialize(): \stdClass {}
}
?>
--EXPECTF--
Fatal error: Foo::__serialize(): Return type must be array when declared in %s on line %d
