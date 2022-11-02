--TEST--
__toBool can only declare bool return type
--FILE--
<?php
class Foo {
    public function __toBool(): string {
    }
}
?>
--EXPECTF--
Fatal error: Foo::__toBool(): Return type must be bool when declared in %s on line %d
