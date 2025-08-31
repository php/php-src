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
Fatal error: Uncaught Error: next(): Argument #1 ($array) could not be passed by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
