--TEST--
Exception during delayed variance autoload
--FILE--
<?php
spl_autoload_register(function($class) {
    echo "$class\n";
    if ($class == 'X') {
        new Y;
    }
    if ($class == 'Y') {
        new Q;
    }
});
class A {
    function method(): X {}
}
class B extends A {
    function method(): Y {}
}
?>
--EXPECTF--
Y
Q

Fatal error: During inheritance of B, while autoloading Y: Uncaught Error: Class "Q" not found in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}('Y')
#1 {main} in %s on line %d
