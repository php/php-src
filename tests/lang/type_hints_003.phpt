--TEST--
ZE2 type
--FILE--
<?php
class T {
    function f(P $p = 42) {
    }
}
?>
--EXPECTF--
Fatal error: T::f(): Parameter #1 ($p) of type P cannot have a default value of type int in %s on line %d
