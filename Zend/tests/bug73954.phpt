--TEST--
Bug #73954 (NAN check fails on Alpine Linux with musl)
--FILE--
<?php

var_dump(NAN);
var_dump(is_nan(NAN));

function takes_int(int $int) {
}

takes_int(log(tan(3.14)));

?>
--EXPECTF--
float(NAN)
bool(true)

Fatal error: Uncaught TypeError: takes_int() expects argument #1 ($int) to be of type int, float given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(9): takes_int(NAN)
#1 {main}
  thrown in %s on line 6
