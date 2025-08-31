--TEST--
Simple math tests
--FILE--
<?php

define('LONG_MAX', is_int(5000000000)? 9223372036854775807 : 0x7FFFFFFF);
define('LONG_MIN', -LONG_MAX - 1);
printf("%d,%d,%d,%d\n",is_int(LONG_MIN  ),is_int(LONG_MAX  ),
                       is_int(LONG_MIN-1),is_int(LONG_MAX+1));

var_dump(1   === abs(-1));
var_dump(1.5 === abs(-1.5));
var_dump(1   === abs("-1"));
var_dump(1.5 === abs("-1.5"));
var_dump(-LONG_MIN+1 === abs(LONG_MIN-1));
var_dump(-LONG_MIN   === abs(LONG_MIN));
var_dump(-(LONG_MIN+1) === abs(LONG_MIN+1));

?>
--EXPECT--
1,1,0,0
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
