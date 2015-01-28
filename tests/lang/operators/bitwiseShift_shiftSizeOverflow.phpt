--TEST--
Test << and >> operators: Shift size overflow
--FILE--
<?php
var_dump(1 << 32);
var_dump(1 << 33);
var_dump(1 << 64);
var_dump(1 << 65);
echo PHP_EOL;
var_dump(1 >> 32);
var_dump(1 >> 33);
var_dump(1 >> 64);
var_dump(1 >> 65);
--EXPECT--
int(4294967296)
int(8589934592)
int(18446744073709551616)
int(36893488147419103232)

int(0)
int(0)
int(0)
int(0)