--TEST--
Negative parse_iv2 edge cases
--FILE--
<?php

var_dump(unserialize('i:-9223372036854775808;'));
var_dump(unserialize('i:-0;'));

?>
--EXPECT--
int(-9223372036854775808)
int(0)
