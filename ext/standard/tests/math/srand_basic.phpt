--TEST--
Test srand() - basic function test for srand()
--FILE--
<?php
echo "*** Testing srand() : basic functionality ***\n";

// Should return NULL if given anything that it can convert to long
// This doesn't actually test what it does with the input :-\
var_dump(srand());
var_dump(srand(500));
var_dump(srand(500.1));
var_dump(srand("500"));
var_dump(srand("500E3"));
var_dump(srand(true));
var_dump(srand(false));
?>
--EXPECTF--
*** Testing srand() : basic functionality ***
NULL
NULL

Deprecated: Implicit conversion from float 500.1 to int loses precision in %s on line %d
NULL
NULL
NULL
NULL
NULL
