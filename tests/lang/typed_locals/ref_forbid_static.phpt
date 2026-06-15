--TEST--
Typed local variables: using a typed local as a static variable is a compile error
--FILE--
<?php
function f() {
    int $x = 1;
    static $x = 0;
}
f();
?>
--EXPECTF--
Fatal error: Cannot use typed local variable $x as static variable in %s on line %d
