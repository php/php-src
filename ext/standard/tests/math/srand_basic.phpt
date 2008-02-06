--TEST--
Maths test for xapic versions of srand()
--FILE--
<?php
// Should return NULL if given anything that it can convert to long
// This doesn't actually test what it does with the input :-\
var_dump(srand());
var_dump(srand(500));
var_dump(srand(500.1));
var_dump(srand("500"));
var_dump(srand("500E3"));
var_dump(srand(true));
var_dump(srand(false));
var_dump(srand(NULL));
?>
--EXPECTF--
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL