--TEST--
PFA errors: not specifying a required param is an error
--FILE--
<?php

function f($a, $b, $c = null) {
}

$f = f(1, c: ?);

?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: f(): Argument #2 ($b) not passed in %s:6
Stack trace:
#0 {main}
  thrown in %s on line %d
