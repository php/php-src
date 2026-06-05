--TEST--
Magic method with incorrect casing is a compile error
--FILE--
<?php
class Foo {
    public function __CONSTRUCT() {
        echo "wrong case construct called\n";
    }
}
?>
--EXPECTF--
Fatal error: Method Foo::__CONSTRUCT() must be spelled __construct() in %s on line %d
