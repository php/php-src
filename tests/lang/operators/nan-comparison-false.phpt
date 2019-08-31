--TEST--
Comparisons with NAN should yield false, even at compile-time
--FILE--
<?php

echo "** CONST\n";
var_dump(0 < NAN);
var_dump(0 <= NAN);
var_dump(0 > NAN);
var_dump(0 >= NAN);

echo "** VAR\n";
$nan = NAN;
var_dump(0 < $nan);
var_dump(0 <= $nan);
var_dump(0 > $nan);
var_dump(0 >= $nan);
--EXPECT--
** CONST
bool(false)
bool(false)
bool(false)
bool(false)
** VAR
bool(false)
bool(false)
bool(false)
bool(false)
