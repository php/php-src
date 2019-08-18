--TEST--
Test stristr() function : error conditions
--FILE--
<?php

/* Prototype: string stristr  ( string $haystack  , mixed $needle  [, bool $before_needle  ] )
   Description: Case-insensitive strstr()
*/
echo "*** Testing stristr() : error conditions ***\n";

echo "\n-- Testing stristr() function with empty haystack --\n";
var_dump( stristr(NULL, "") );

echo "\n-- Testing stristr() function with empty needle --\n";
var_dump( stristr("Hello World", "") );

?>
===DONE===
--EXPECT--
*** Testing stristr() : error conditions ***

-- Testing stristr() function with empty haystack --
string(0) ""

-- Testing stristr() function with empty needle --
string(11) "Hello World"
===DONE===
