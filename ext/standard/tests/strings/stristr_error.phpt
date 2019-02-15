--TEST--
Test stristr() function : error conditions
--FILE--
<?php

/* Prototype: string stristr  ( string $haystack  , mixed $needle  [, bool $before_needle  ] )
   Description: Case-insensitive strstr()
*/
echo "*** Testing stristr() : error conditions ***\n";

echo "\n-- Testing stristr() function with no arguments --\n";
var_dump( stristr() );
var_dump( stristr("") );

echo "\n-- Testing stristr() function with no needle --\n";
var_dump( stristr("Hello World") );  // without "needle"

echo "\n-- Testing stristr() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( stristr("Hello World",  "World", true, $extra_arg) );

echo "\n-- Testing stristr() function with empty haystack --\n";
var_dump( stristr(NULL, "") );

echo "\n-- Testing stristr() function with empty needle --\n";
var_dump( stristr("Hello World", "") );

?>
===DONE===
--EXPECTF--
*** Testing stristr() : error conditions ***

-- Testing stristr() function with no arguments --

Warning: stristr() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: stristr() expects at least 2 parameters, 1 given in %s on line %d
NULL

-- Testing stristr() function with no needle --

Warning: stristr() expects at least 2 parameters, 1 given in %s on line %d
NULL

-- Testing stristr() function with more than expected no. of arguments --

Warning: stristr() expects at most 3 parameters, 4 given in %s on line %d
NULL

-- Testing stristr() function with empty haystack --

Warning: stristr(): Empty needle in %s on line %d
bool(false)

-- Testing stristr() function with empty needle --

Warning: stristr(): Empty needle in %s on line %d
bool(false)
===DONE===
