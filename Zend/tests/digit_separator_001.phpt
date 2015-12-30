--TEST--
Valid use of digit separator
--FILE--
<?php

var_dump(1_000_000 === 1000000);
var_dump(3.141_592 === 3.141592);
var_dump(0x02_56_12 === 0x025612);
var_dump(0b0010_1101 === 0b00101101);
var_dump(0267_3432 === 02673432);
var_dump(1_123.456_7e2 === 1123.4567e2);
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
