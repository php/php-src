--TEST--
Declaring a magic method with wrong case is a compile error (__tostring)
--FILE--
<?php
class Foo {
    public function __tostring(): string { return 'x'; }
}
?>
--EXPECTF--
Fatal error: Method Foo::__tostring() must be spelled __toString() in %s on line %d
