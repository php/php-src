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

Warning: Uncaught Error: Class "Q" not found in %s:%d
Stack trace:
#0 %s(%d): {closure}('Y')
#1 {main}
  thrown in %s on line %d

Fatal error: Could not check compatibility between B::method(): Y and A::method(): X, because class Y is not available in %s on line %d
