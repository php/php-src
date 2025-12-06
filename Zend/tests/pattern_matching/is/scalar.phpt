--TEST--
Scalar pattern matching
--FILE--
<?php

var_dump(1 is 1);
var_dump(1.0 is 1);
var_dump('1' is 1);
var_dump(1 is 1.0);
var_dump(1.0 is 1.0);
var_dump('1' is 1.0);
var_dump(1 is '1');
var_dump(1.0 is '1');
var_dump('1' is '1');

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
