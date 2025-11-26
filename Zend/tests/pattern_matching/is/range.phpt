--TEST--
Range pattern
--FILE--
<?php

var_dump(-1 is 0..<10);
var_dump(0  is 0..<10);
var_dump(1  is 0..<10);
var_dump(9  is 0..<10);
var_dump(10 is 0..<10);

var_dump(-1 is 0..=10);
var_dump(0  is 0..=10);
var_dump(1  is 0..=10);
var_dump(9  is 0..=10);
var_dump(10 is 0..=10);

var_dump(10.0 is 0..<10);
var_dump(10.0 is 0..=10);

var_dump(9.5  is ..<10);
var_dump(9.5  is ..=10);
var_dump(10.0 is ..<10);
var_dump(10.0 is ..=10);
var_dump(10.5 is ..<10);
var_dump(10.5 is ..=10);

var_dump(0.5  is 0..<);
var_dump(0.5  is 0..=);
var_dump(0.0  is 0..<);
var_dump(0.0  is 0..=);
var_dump(-0.5 is 0..<);
var_dump(-0.5 is 0..=);

var_dump(0.09 is 0.0..=0.1);
var_dump(0.1  is 0.0..=0.1);
var_dump(0.11 is 0.0..=0.1);

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
