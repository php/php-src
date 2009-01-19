--TEST--
Test stristr() function : error conditions 
--FILE--
<?php

/* Prototype: string stristr ( string $haystack, string $needle );
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
var_dump( stristr("Hello World",  "World", $extra_arg) );

echo "\n-- Testing stristr() function with empty haystack --\n";
var_dump( stristr(NULL, "") );

echo "\n-- Testing stristr() function with empty needle --\n";
var_dump( stristr("Hello World", "") );

?>
===DONE===
--EXPECTF--
*** Testing stristr() : error conditions ***

-- Testing stristr() function with no arguments --

Warning: Wrong parameter count for stristr() in %s on line %d
NULL

Warning: Wrong parameter count for stristr() in %s on line %d
NULL

-- Testing stristr() function with no needle --

Warning: Wrong parameter count for stristr() in %s on line %d
NULL

-- Testing stristr() function with more than expected no. of arguments --

Warning: Wrong parameter count for stristr() in %s on line %d
NULL

-- Testing stristr() function with empty haystack --

Warning: stristr(): Empty delimiter in %s on line %d
bool(false)

-- Testing stristr() function with empty needle --

Warning: stristr(): Empty delimiter in %s on line %d
bool(false)
===DONE===
