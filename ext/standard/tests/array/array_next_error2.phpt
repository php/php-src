--TEST--
next - ensure we cannot pass a temporary
--FILE--
<?php
function f() {
    return array(1, 2);
}
var_dump(next(array(1, 2)));
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot pass parameter 1 by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
