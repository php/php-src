--TEST--
Test << and >> operators: Negative shifts
--FILE--
<?php
var_dump(2 << -1);
var_dump(2 >> -1);
--EXPECTF--
Warning: Bit shift by negative number in %s on line %d

Warning: Bit shift by negative number in %s on line %d
bool(false)
bool(false)