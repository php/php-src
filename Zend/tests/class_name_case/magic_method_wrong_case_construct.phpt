--TEST--
Declaring a magic method with wrong case is a compile error (__Construct)
--FILE--
<?php
class Foo {
    public function __Construct() {}
}
?>
--EXPECTF--
Fatal error: Method Foo::__Construct() must be spelled __construct() in %s on line %d
