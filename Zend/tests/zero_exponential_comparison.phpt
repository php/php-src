--TEST--
Comparison of zero exponential strings
--FILE--
<?php

// One side is not a zero exponential
var_dump("0" == "0e0");
var_dump("0.0" == "0e0");
var_dump(".0" == "0e0");
// Zero exponentials are exactly identical
var_dump("0e0" == "0e0");
var_dump("0e00" == "0e00");
echo "\n";

// Both sides are zero exponentials, but not identical
var_dump("0e0" == "0e00");
var_dump("0e0" == "0e1");
var_dump("0e10" == "0e2");
echo "\n";

// Fallback to lexicographical order
var_dump("0e0" < "0e1");
var_dump("0e0" > "0e1");
var_dump("0e10" < "0e2");
var_dump("0e10" > "0e2");

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

bool(false)
bool(false)
bool(false)

bool(true)
bool(false)
bool(true)
bool(false)
