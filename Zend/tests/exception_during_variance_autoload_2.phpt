--TEST--
Exception during delayed variance autoload (variation 2)
--FILE--
<?php
spl_autoload_register(function() {
    class Y {}
    throw new Exception;
});
class A {
    function method(): object {}
}
class B extends A {
    function method(): Y {}
}
?>
--EXPECTF--
Fatal error: During inheritance of B, while autoloading Y: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): {closure}('Y')
#1 {main} in %s on line %d
