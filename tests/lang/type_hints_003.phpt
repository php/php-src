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
Fatal error: Cannot use int as default value for parameter $p of type P in %s on line %d
